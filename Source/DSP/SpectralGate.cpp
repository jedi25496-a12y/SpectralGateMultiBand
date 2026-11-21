/*
    SpectralGate.cpp — Per-bin noise gate in frequency domain
*/

#include "SpectralGate.h"

namespace vybecode {

void SpectralGate::prepare(const ModuleSpec& spec) {
    currentSampleRate = spec.sampleRate;
    fft = std::make_unique<juce::dsp::FFT>(fftOrder);

    windowFunction.resize(static_cast<size_t>(fftSize));
    for (int i = 0; i < fftSize; ++i)
        windowFunction[static_cast<size_t>(i)] =
            0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * static_cast<float>(i) / static_cast<float>(fftSize)));

    channels.resize(static_cast<size_t>(spec.numChannels));
    for (auto& ch : channels) {
        ch.inputBuffer.assign(static_cast<size_t>(fftSize), 0.0f);
        ch.fftData.resize(static_cast<size_t>(fftSize * 2), 0.0f);
        ch.binGainSmoothed.assign(static_cast<size_t>(fftSize / 2 + 1), 1.0f);
        ch.overlapAddBuffer.assign(static_cast<size_t>(fftSize), 0.0f);
        ch.inputWritePos = 0;
        ch.outputReadPos = 0;
        ch.samplesUntilNextHop = 0;
    }
}

void SpectralGate::processHop(ChannelState& ch, float thresholdLinear, float smoothing) {
    const int halfN = fftSize / 2 + 1;

    // Window and copy
    std::fill(ch.fftData.begin(), ch.fftData.end(), 0.0f);
    for (int i = 0; i < fftSize; ++i) {
        int readIdx = (ch.inputWritePos - fftSize + i + fftSize) % fftSize;
        ch.fftData[static_cast<size_t>(i)] =
            ch.inputBuffer[static_cast<size_t>(readIdx)] * windowFunction[static_cast<size_t>(i)];
    }

    fft->performRealOnlyForwardTransform(ch.fftData.data(), true);

    // Per-bin gating
    for (int bin = 0; bin < halfN; ++bin) {
        float re = ch.fftData[static_cast<size_t>(bin * 2)];
        float im = ch.fftData[static_cast<size_t>(bin * 2 + 1)];
        float mag = std::sqrt(re * re + im * im);

        // Gate: if magnitude below threshold, attenuate
        float targetGain = (mag > thresholdLinear) ? 1.0f : 0.0f;

        // Smooth the gate gain to avoid clicks
        float& smoothedGain = ch.binGainSmoothed[static_cast<size_t>(bin)];
        smoothedGain += (1.0f - smoothing) * (targetGain - smoothedGain);

        ch.fftData[static_cast<size_t>(bin * 2)]     = re * smoothedGain;
        ch.fftData[static_cast<size_t>(bin * 2 + 1)] = im * smoothedGain;
    }

    fft->performRealOnlyInverseTransform(ch.fftData.data());

    for (int i = 0; i < fftSize; ++i) {
        int outIdx = (ch.outputReadPos + i) % fftSize;
        ch.overlapAddBuffer[static_cast<size_t>(outIdx)] +=
            ch.fftData[static_cast<size_t>(i)] * windowFunction[static_cast<size_t>(i)];
    }
}

void SpectralGate::process(juce::AudioBuffer<float>& buffer) {
    float threshDb = (thresholdDbParam != nullptr) ? thresholdDbParam->load() : -30.0f;
    float smoothing = (smoothingParam != nullptr) ? smoothingParam->load() : 0.5f;
    float mix = (mixParam != nullptr) ? mixParam->load() : 1.0f;

    float threshLinear = std::pow(10.0f, threshDb / 20.0f);

    const int numSamples = buffer.getNumSamples();
    const int numChannels = juce::jmin(buffer.getNumChannels(), static_cast<int>(channels.size()));

    for (int ch = 0; ch < numChannels; ++ch) {
        auto& state = channels[static_cast<size_t>(ch)];
        float* data = buffer.getWritePointer(ch);

        for (int s = 0; s < numSamples; ++s) {
            float dry = data[s];

            state.inputBuffer[static_cast<size_t>(state.inputWritePos)] = dry;
            state.inputWritePos = (state.inputWritePos + 1) % fftSize;

            float wet = state.overlapAddBuffer[static_cast<size_t>(state.outputReadPos)];
            state.overlapAddBuffer[static_cast<size_t>(state.outputReadPos)] = 0.0f;
            state.outputReadPos = (state.outputReadPos + 1) % fftSize;

            if (--state.samplesUntilNextHop <= 0) {
                processHop(state, threshLinear, smoothing);
                state.samplesUntilNextHop = hopSize;
            }

            data[s] = dry * (1.0f - mix) + wet * mix;
        }
    }
}

void SpectralGate::reset() {
    for (auto& ch : channels) {
        std::fill(ch.inputBuffer.begin(), ch.inputBuffer.end(), 0.0f);
        std::fill(ch.overlapAddBuffer.begin(), ch.overlapAddBuffer.end(), 0.0f);
        std::fill(ch.binGainSmoothed.begin(), ch.binGainSmoothed.end(), 1.0f);
        ch.inputWritePos = 0;
        ch.outputReadPos = 0;
        ch.samplesUntilNextHop = 0;
    }
}

std::vector<std::unique_ptr<juce::RangedAudioParameter>>
SpectralGate::createParameters(const juce::String& prefix) {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "threshold_db", 1},
        prefix + "Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
        -30.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "smoothing", 1},
        prefix + "Smoothing",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "mix", 1},
        prefix + "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        1.0f));

    return params;
}

void SpectralGate::updateFromAPVTS(juce::AudioProcessorValueTreeState& apvts,
                                    const juce::String& prefix) {
    thresholdDbParam = getParam(apvts, prefix, "threshold_db");
    smoothingParam = getParam(apvts, prefix, "smoothing");
    mixParam = getParam(apvts, prefix, "mix");
}

} // namespace vybecode

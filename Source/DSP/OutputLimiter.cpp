/*
    OutputLimiter.cpp — Safety limiter for output stage
*/

#include "OutputLimiter.h"

namespace vybecode {

void OutputLimiter::prepare(const ModuleSpec& spec) {
    sampleRate = spec.sampleRate;
    currentGain = 1.0f;
}

void OutputLimiter::process(juce::AudioBuffer<float>& buffer) {
    const float ceilingDb = ceilingParam ? ceilingParam->load() : -0.3f;
    const float recoveryMs = recoveryParam ? recoveryParam->load() : 50.0f;

    const float ceiling = juce::Decibels::decibelsToGain(ceilingDb);

    // Recovery coefficient: how fast gain returns to 1.0
    // Larger recovery time = slower return
    const float recoveryCoeff = std::exp(-1.0f / (static_cast<float>(sampleRate)
                                                   * recoveryMs * 0.001f));

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int sample = 0; sample < numSamples; ++sample) {
        // Find peak across channels for this sample
        float peak = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch) {
            float absSample = std::abs(buffer.getReadPointer(ch)[sample]);
            if (absSample > peak)
                peak = absSample;
        }

        // Calculate required gain reduction
        float requiredGain = (peak > ceiling) ? (ceiling / peak) : 1.0f;

        // Smooth gain: instant attack, smooth recovery
        if (requiredGain < currentGain) {
            currentGain = requiredGain;  // Instant attack
        } else {
            // Smooth recovery
            currentGain = currentGain * recoveryCoeff + requiredGain * (1.0f - recoveryCoeff);
        }

        // Apply gain
        for (int ch = 0; ch < numChannels; ++ch)
            buffer.getWritePointer(ch)[sample] *= currentGain;
    }
}

void OutputLimiter::reset() {
    currentGain = 1.0f;
}

std::vector<std::unique_ptr<juce::RangedAudioParameter>>
OutputLimiter::createParameters(const juce::String& prefix) {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "ceiling_db", 1},
        prefix + "Ceiling",
        juce::NormalisableRange<float>(-6.0f, 0.0f, 0.1f),
        -0.3f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "recovery_ms", 1},
        prefix + "Recovery",
        juce::NormalisableRange<float>(10.0f, 500.0f, 1.0f, 0.4f),
        50.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    return params;
}

void OutputLimiter::updateFromAPVTS(juce::AudioProcessorValueTreeState& apvts,
                                       const juce::String& prefix) {
    ceilingParam = getParam(apvts, prefix, "ceiling_db");
    recoveryParam = getParam(apvts, prefix, "recovery_ms");
}

} // namespace vybecode

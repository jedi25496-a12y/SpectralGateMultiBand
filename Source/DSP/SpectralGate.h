/*
    SpectralGate.h — Per-bin noise gate in frequency domain
    Category: Spectral
    Parameters: threshold_db (-60 to 0, default -30), smoothing (0-1, default 0.5),
                mix (0-1, default 1.0)
*/

#pragma once
#include "DSPModule.h"
#include <cmath>
#include <vector>

namespace vybecode {

class SpectralGate : public DSPModule {
public:
    void prepare(const ModuleSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>>
        createParameters(const juce::String& prefix) override;
    void updateFromAPVTS(juce::AudioProcessorValueTreeState& apvts,
                          const juce::String& prefix) override;

    juce::String getModuleID() const override { return "spectral_gate"; }
    juce::String getModuleName() const override { return "Spectral Gate"; }
    int getLatencySamples() const override { return fftSize; }

private:
    std::atomic<float>* thresholdDbParam = nullptr;
    std::atomic<float>* smoothingParam = nullptr;
    std::atomic<float>* mixParam = nullptr;

    double currentSampleRate = 44100.0;
    static constexpr int fftOrder = 11;
    static constexpr int fftSize = 1 << fftOrder; // 2048
    static constexpr int hopSize = fftSize / 4;

    std::unique_ptr<juce::dsp::FFT> fft;

    struct ChannelState {
        std::vector<float> inputBuffer;
        std::vector<float> fftData;
        std::vector<float> binGainSmoothed;
        std::vector<float> overlapAddBuffer;
        int inputWritePos = 0;
        int outputReadPos = 0;
        int samplesUntilNextHop = 0;
    };

    std::vector<ChannelState> channels;
    std::vector<float> windowFunction;

    void processHop(ChannelState& ch, float thresholdLinear, float smoothing);
};

} // namespace vybecode

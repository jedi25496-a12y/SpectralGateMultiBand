/*
    OutputLimiter.h — Safety limiter for output stage
    Category: Utility
    Parameters: ceiling_db (-6 to 0, default -0.3), recovery_ms (10-500, default 50)

    Simple hard clip at ceiling with smoothed gain recovery.
*/

#pragma once
#include "DSPModule.h"
#include <cmath>

namespace vybecode {

class OutputLimiter : public DSPModule {
public:
    void prepare(const ModuleSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>>
        createParameters(const juce::String& prefix) override;
    void updateFromAPVTS(juce::AudioProcessorValueTreeState& apvts,
                          const juce::String& prefix) override;

    juce::String getModuleID() const override { return "output_limiter"; }
    juce::String getModuleName() const override { return "Output Limiter"; }

private:
    std::atomic<float>* ceilingParam = nullptr;
    std::atomic<float>* recoveryParam = nullptr;

    double sampleRate = 44100.0;
    float currentGain = 1.0f;  // Gain reduction envelope
};

} // namespace vybecode

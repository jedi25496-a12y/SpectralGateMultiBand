#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class SpectralGateMultiBandEditor : public juce::AudioProcessorEditor
{
public:
    explicit SpectralGateMultiBandEditor(SpectralGateMultiBandProcessor&);
    ~SpectralGateMultiBandEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SpectralGateMultiBandProcessor& processorRef;

    // ── Controls ─────────────────────────────────────────
    juce::Label sectionLabel0;
    juce::Label sectionLabel1;
    juce::Label sectionLabel2;
    juce::Slider specgate_threshold_dbSlider;
    juce::Slider specgate_mixSlider;
    juce::Slider input_gain_dbSlider;
    juce::Slider output_ceiling_dbSlider;
    juce::Slider specgate_smoothingSlider;
    juce::Slider output_recovery_msSlider;


    // ── Attachments (declared AFTER sliders for correct destruction order) ──
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> specgate_threshold_dbAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> specgate_mixAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> input_gain_dbAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> output_ceiling_dbAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> specgate_smoothingAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> output_recovery_msAttach;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectralGateMultiBandEditor)
};

#include "PluginEditor.h"

SpectralGateMultiBandEditor::SpectralGateMultiBandEditor(SpectralGateMultiBandProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    sectionLabel0.setText("Specgate", juce::dontSendNotification);
    sectionLabel0.setFont(juce::FontOptions(14.0f, juce::Font::bold));
    sectionLabel0.setColour(juce::Label::textColourId, juce::Colour(0xff0066ff));
    addAndMakeVisible(sectionLabel0);
    sectionLabel1.setText("Input", juce::dontSendNotification);
    sectionLabel1.setFont(juce::FontOptions(14.0f, juce::Font::bold));
    sectionLabel1.setColour(juce::Label::textColourId, juce::Colour(0xff0066ff));
    addAndMakeVisible(sectionLabel1);
    sectionLabel2.setText("Output", juce::dontSendNotification);
    sectionLabel2.setFont(juce::FontOptions(14.0f, juce::Font::bold));
    sectionLabel2.setColour(juce::Label::textColourId, juce::Colour(0xff0066ff));
    addAndMakeVisible(sectionLabel2);
    specgate_threshold_dbSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    specgate_threshold_dbSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    specgate_threshold_dbSlider.setName("specgate_threshold_db");
    addAndMakeVisible(specgate_threshold_dbSlider);
    specgate_mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    specgate_mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    specgate_mixSlider.setName("specgate_mix");
    addAndMakeVisible(specgate_mixSlider);
    input_gain_dbSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    input_gain_dbSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    input_gain_dbSlider.setName("input_gain_db");
    addAndMakeVisible(input_gain_dbSlider);
    output_ceiling_dbSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    output_ceiling_dbSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    output_ceiling_dbSlider.setName("output_ceiling_db");
    addAndMakeVisible(output_ceiling_dbSlider);
    specgate_smoothingSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    specgate_smoothingSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    specgate_smoothingSlider.setName("specgate_smoothing");
    addAndMakeVisible(specgate_smoothingSlider);
    output_recovery_msSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    output_recovery_msSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    output_recovery_msSlider.setName("output_recovery_ms");
    addAndMakeVisible(output_recovery_msSlider);


    specgate_threshold_dbAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "specgate_threshold_db", specgate_threshold_dbSlider);
    specgate_mixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "specgate_mix", specgate_mixSlider);
    input_gain_dbAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "input_gain_db", input_gain_dbSlider);
    output_ceiling_dbAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "output_ceiling_db", output_ceiling_dbSlider);
    specgate_smoothingAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "specgate_smoothing", specgate_smoothingSlider);
    output_recovery_msAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "output_recovery_ms", output_recovery_msSlider);


    setSize(820, 480);
}

SpectralGateMultiBandEditor::~SpectralGateMultiBandEditor() {}

void SpectralGateMultiBandEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xfff5f5f7));


    g.setColour(juce::Colour(0xff0066ff));
    g.setFont(juce::FontOptions(20.0f));
    g.drawFittedText("SpectralGateMultiBand", getLocalBounds().removeFromTop(40),
                     juce::Justification::centred, 1);
}

void SpectralGateMultiBandEditor::resized()
{
    auto area = getLocalBounds().reduced(16);
    area.removeFromTop(44); // Title space

    sectionLabel0.setBounds(14, 58, 792, 18);
    sectionLabel1.setBounds(14, 166, 792, 18);
    sectionLabel2.setBounds(14, 274, 792, 18);
    specgate_threshold_dbSlider.setBounds(14, 82, 58, 66);
    specgate_mixSlider.setBounds(80, 82, 58, 66);
    input_gain_dbSlider.setBounds(14, 190, 58, 66);
    output_ceiling_dbSlider.setBounds(14, 298, 58, 66);
    specgate_smoothingSlider.setBounds(8, 376, 189, 78);
    output_recovery_msSlider.setBounds(213, 376, 189, 78);

}

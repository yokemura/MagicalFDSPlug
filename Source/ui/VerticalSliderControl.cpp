/*
  ==============================================================================

    VerticalSliderControl.cpp

  ==============================================================================
*/

#include "VerticalSliderControl.h"

namespace MagicalFDS::UI
{
VerticalSliderControl::VerticalSliderControl (juce::AudioProcessorValueTreeState* apvtsOrNull,
                                              const juce::String& paramIdOrEmpty,
                                              const juce::String& caption)
{
    slider.setSliderStyle (juce::Slider::LinearVertical);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible (slider);

    label.setText (caption, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setMinimumHorizontalScale (0.8f);
    addAndMakeVisible (label);

    if (apvtsOrNull != nullptr && paramIdOrEmpty.isNotEmpty())
        attach (*apvtsOrNull, paramIdOrEmpty);
}

void VerticalSliderControl::attach (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramId)
{
    attachment = std::make_unique<SliderAttachment> (apvts, paramId, slider);
}

void VerticalSliderControl::detach()
{
    attachment.reset();
}

void VerticalSliderControl::resized()
{
    auto r = getLocalBounds();
    auto labelArea = r.removeFromBottom (18);
    label.setBounds (labelArea);
    slider.setBounds (r.reduced (0, 2));
}
} // namespace MagicalFDS::UI


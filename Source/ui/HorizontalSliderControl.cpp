/*
  ==============================================================================

    HorizontalSliderControl.cpp

  ==============================================================================
*/

#include "HorizontalSliderControl.h"

#include "../LayoutConstants.h"

namespace MagicalFDS::UI
{
HorizontalSliderControl::HorizontalSliderControl (juce::AudioProcessorValueTreeState& apvts,
                                                    const juce::String& paramId,
                                                    const juce::String& name,
                                                    bool bipolarFromCentre)
{
    label.setText (name, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centredRight);
    addAndMakeVisible (label);

    slider.setSliderStyle (juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 80, Layout::rowHeight);
    addAndMakeVisible (slider);

    if (bipolarFromCentre)
        slider.getProperties().set ("magicalFdsBipolarMorph", true);

    attachment = std::make_unique<SliderAttachment> (apvts, paramId, slider);
}

void HorizontalSliderControl::resized()
{
    auto r = getLocalBounds();

    auto labelArea = r.removeFromLeft (Layout::labelColumnWidth);
    label.setBounds (labelArea.reduced (0, 1));

    r.removeFromLeft (Layout::labelControlGap);
    slider.setBounds (r.reduced (0, 1));
}
} // namespace MagicalFDS::UI


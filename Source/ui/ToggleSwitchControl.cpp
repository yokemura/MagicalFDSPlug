/*
  ==============================================================================

    ToggleSwitchControl.cpp

  ==============================================================================
*/

#include "ToggleSwitchControl.h"

#include "../LayoutConstants.h"

namespace MagicalFDS::UI
{
ToggleSwitchControl::ToggleSwitchControl (juce::AudioProcessorValueTreeState& apvts,
                                          const juce::String& paramId,
                                          const juce::String& name)
{
    label.setText (name, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centredRight);
    addAndMakeVisible (label);

    addAndMakeVisible (toggle);
    attachment = std::make_unique<ButtonAttachment> (apvts, paramId, toggle);
}

void ToggleSwitchControl::resized()
{
    auto r = getLocalBounds();

    auto labelArea = r.removeFromLeft (Layout::labelColumnWidth);
    label.setBounds (labelArea.reduced (0, 1));

    r.removeFromLeft (Layout::labelControlGap);
    toggle.setBounds (r.removeFromLeft (Layout::rowHeight).reduced (0, 2));
}
} // namespace MagicalFDS::UI


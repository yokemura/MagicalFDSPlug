/*
  ==============================================================================

    ChoiceControl.cpp

  ==============================================================================
*/

#include "ChoiceControl.h"

#include "../LayoutConstants.h"

namespace MagicalFDS::UI
{
ChoiceControl::ChoiceControl (juce::AudioProcessorValueTreeState& apvts,
                              const juce::String& paramId,
                              const juce::String& name)
{
    label.setText (name, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (label);

    addAndMakeVisible (comboBox);

    // ComboBoxAttachment does not populate items; mirror AudioParameterChoice strings.
    if (auto* p = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (paramId)))
        comboBox.addItemList (p->choices, 1);

    attachment = std::make_unique<ComboBoxAttachment> (apvts, paramId, comboBox);
}

void ChoiceControl::resized()
{
    auto r = getLocalBounds();

    auto labelArea = r.removeFromLeft (Layout::labelColumnWidth);
    label.setBounds (labelArea.reduced (0, 1));

    r.removeFromLeft (Layout::labelControlGap);
    comboBox.setBounds (r.reduced (0, 2));
}
} // namespace MagicalFDS::UI


/*
  ==============================================================================

    SectionLabel.h
    A simple label for section headers (Magical8bitPlug2-like).

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace MagicalFDS::UI
{
class SectionLabel final : public juce::Label
{
public:
    SectionLabel()
    {
        setJustificationType (juce::Justification::centredLeft);
        setFont (juce::Font (juce::FontOptions (17.0f)));
    }

    explicit SectionLabel (const juce::String& text) : SectionLabel()
    {
        setText (text, juce::dontSendNotification);
    }
};
} // namespace MagicalFDS::UI


/*
  ==============================================================================

    MagicalFDSLookAndFeel.cpp

  ==============================================================================
*/

#include "MagicalFDSLookAndFeel.h"

namespace MagicalFDS
{
void MagicalFDSLookAndFeel::applyColorScheme (const ColorScheme& scheme)
{
    using Id = juce::ComboBox::ColourIds;

    setColour (juce::ResizableWindow::backgroundColourId, scheme.background);
    setColour (juce::Label::textColourId, scheme.main);
    setColour (juce::Label::outlineColourId, scheme.genericBorder);
    setColour (juce::TextEditor::textColourId, scheme.main);
    setColour (juce::TextEditor::backgroundColourId, scheme.textBoxFill);
    setColour (juce::TextEditor::outlineColourId, scheme.genericBorder);
    setColour (juce::Slider::thumbColourId, scheme.accent);
    setColour (juce::Slider::trackColourId, scheme.mainThinLine);
    setColour (juce::Slider::backgroundColourId, scheme.boxFill);
    setColour (juce::Slider::textBoxTextColourId, scheme.main);
    setColour (juce::Slider::textBoxBackgroundColourId, scheme.textBoxFill);
    setColour (juce::Slider::textBoxOutlineColourId, scheme.genericBorder);
    setColour (Id::backgroundColourId, scheme.textBoxFill);
    setColour (Id::outlineColourId, scheme.genericBorder);
    setColour (Id::textColourId, scheme.main);
    setColour (Id::arrowColourId, scheme.main);
    setColour (juce::ToggleButton::textColourId, scheme.main);
    setColour (juce::ToggleButton::tickColourId, scheme.accent);
    setColour (juce::ToggleButton::tickDisabledColourId, scheme.mainDarkened);
    setColour (juce::TextButton::buttonColourId, scheme.boxFill);
    setColour (juce::TextButton::textColourOffId, scheme.main);
    setColour (juce::TextButton::textColourOnId, scheme.reversedForeground);
    setColour (juce::ComboBox::backgroundColourId, scheme.textBoxFill);
    setColour (juce::PopupMenu::backgroundColourId, scheme.textBoxFill);
    setColour (juce::PopupMenu::textColourId, scheme.main);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, scheme.main);
    setColour (juce::PopupMenu::highlightedTextColourId, scheme.reversedForeground);
}
} // namespace MagicalFDS

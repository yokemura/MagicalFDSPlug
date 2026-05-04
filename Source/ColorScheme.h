/*
  ==============================================================================

    ColorScheme.h
    Theme palette (ported from Magical8bitPlug2).

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace MagicalFDS
{
enum ColorSchemeType
{
    kColorSchemeYmck = 0,
    kColorSchemeYmckDark,
    kColorSchemeFamicom,        // Japan
    kColorSchemeNes,            // Worldwide
    kColorSchemeMonotoneLight,
    kColorSchemeMonotoneDark,
    kColorSchemeFds
};

struct ColorScheme
{
    explicit ColorScheme (ColorSchemeType type);

    juce::Colour background;
    juce::Colour textBoxFill;
    juce::Colour boxFill;
    juce::Colour main;
    juce::Colour mainThinLine;
    juce::Colour mainDarkened;
    juce::Colour accent;
    juce::Colour reversedForeground;
    juce::Colour genericBorder;
    juce::Colour warning;
};
} // namespace MagicalFDS


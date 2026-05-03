/*
  ==============================================================================

    MagicalFDSLookAndFeel.h
    Maps MagicalFDS::ColorScheme onto JUCE widget colours.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ColorScheme.h"

namespace MagicalFDS
{
class MagicalFDSLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    void applyColorScheme (const ColorScheme& scheme);
};
} // namespace MagicalFDS

/*
  ==============================================================================

    GlobalSectionComponent.h
    Volume, polyphony, bend (left column); color theme, lowpass (right column) — 3:1 split.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ChoiceControl.h"
#include "HorizontalSliderControl.h"
#include "SectionLabel.h"
#include "ToggleSwitchControl.h"

namespace MagicalFDS::UI
{
class GlobalSectionComponent final : public juce::Component
{
public:
    explicit GlobalSectionComponent (juce::AudioProcessorValueTreeState& apvts);

    void resized() override;

private:
    SectionLabel heading { "Global" };

    HorizontalSliderControl gain;
    HorizontalSliderControl polyphony;
    HorizontalSliderControl bendRange;
    ChoiceControl         colorTheme;
    ToggleSwitchControl   lowpass;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlobalSectionComponent)
};
} // namespace MagicalFDS::UI

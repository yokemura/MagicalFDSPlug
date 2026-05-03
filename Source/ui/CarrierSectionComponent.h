/*
  ==============================================================================

    CarrierSectionComponent.h
    Section label, ADSR, mode, wave display, mode-specific control panel.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "CarrierControlPanel.h"
#include "ChoiceControl.h"
#include "HorizontalSliderControl.h"
#include "SectionLabel.h"
#include "WaveDisplayComponent.h"

namespace MagicalFDS::UI
{
class CarrierSectionComponent final : public juce::Component
{
public:
    explicit CarrierSectionComponent (juce::AudioProcessorValueTreeState& apvts);

    void resized() override;

private:
    void syncModeFromParameter();

    juce::AudioProcessorValueTreeState& apvts;

    SectionLabel heading { "Carrier" };

    HorizontalSliderControl carrierA;
    HorizontalSliderControl carrierD;
    HorizontalSliderControl carrierS;
    HorizontalSliderControl carrierR;

    ChoiceControl carrierMode;

    WaveDisplayComponent waveDisplay;
    CarrierControlPanel  controlPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CarrierSectionComponent)
};
} // namespace MagicalFDS::UI

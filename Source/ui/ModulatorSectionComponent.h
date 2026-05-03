/*
  ==============================================================================

    ModulatorSectionComponent.h
    Modulator wave preview, wave type (triangle / saw), rate, depth, ADSR.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "HorizontalSliderControl.h"
#include "SectionLabel.h"
#include "WaveDisplayComponent.h"

namespace MagicalFDS::UI
{
class ModulatorSectionComponent final : public juce::Component,
                                        private juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit ModulatorSectionComponent (juce::AudioProcessorValueTreeState& apvts);
    ~ModulatorSectionComponent() override;

    void resized() override;

private:
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    void syncWaveButtonsFromParameter();

    void setModWaveIndex (int index);

    juce::AudioProcessorValueTreeState& apvts;

    SectionLabel heading { "Modulator" };

    WaveDisplayComponent waveDisplay;

    juce::ToggleButton modWaveTri { "Triangle" };
    juce::ToggleButton modWaveSaw { "Saw" };

    HorizontalSliderControl modRate;
    HorizontalSliderControl modDepth;

    HorizontalSliderControl modA;
    HorizontalSliderControl modD;
    HorizontalSliderControl modS;
    HorizontalSliderControl modR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModulatorSectionComponent)
};
} // namespace MagicalFDS::UI

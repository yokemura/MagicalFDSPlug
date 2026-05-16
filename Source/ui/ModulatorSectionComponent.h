/*
  ==============================================================================

    ModulatorSectionComponent.h
    Modulator wave preview, wave type (triangle / saw), rate, depth, ADSR.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ChoiceControl.h"
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

    void clampModRateToModeRange();
    void syncModRateSliderFromParameter();
    void pushModRateSliderToParameter();

    void clampModDepthToModeRange();
    void syncModDepthSliderFromParameter();
    void pushModDepthSliderToParameter();

    void setModWaveIndex (int index);

    juce::AudioProcessorValueTreeState& apvts;

    SectionLabel heading { "Modulator" };

    WaveDisplayComponent waveDisplay;

    juce::ToggleButton modWaveTri    { "Triangle" };
    juce::ToggleButton modWaveSaw    { "Saw" };
    juce::ToggleButton modWaveSine   { "Sine" };
    juce::ToggleButton modWaveSquare { "Square" };
    juce::ToggleButton modWaveRise   { "Rise" };
    juce::ToggleButton modWaveFall   { "Fall" };

    ChoiceControl modRateUseControl;
    juce::Label modRateLabel;
    juce::Slider modRateSlider;

    juce::Label modDepthLabel;
    juce::Slider modDepthSlider;

    HorizontalSliderControl modA;
    HorizontalSliderControl modD;
    HorizontalSliderControl modS;
    HorizontalSliderControl modR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModulatorSectionComponent)
};
} // namespace MagicalFDS::UI

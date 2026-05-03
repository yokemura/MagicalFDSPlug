/*
  ==============================================================================

    MagicalFDSPlug editor — global / carrier / modulator layout (see docs/UI-Specification.md).

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "MagicalFDSLookAndFeel.h"
#include "PluginProcessor.h"
#include "ui/CarrierSectionComponent.h"
#include "ui/GlobalSectionComponent.h"
#include "ui/ModulatorSectionComponent.h"

//==============================================================================
class NewProjectAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                             private juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    void applyThemeFromParameters();

    NewProjectAudioProcessor& audioProcessor;

    MagicalFDS::MagicalFDSLookAndFeel lookAndFeel;

    MagicalFDS::UI::GlobalSectionComponent   globalSection;
    MagicalFDS::UI::CarrierSectionComponent  carrierSection;
    MagicalFDS::UI::ModulatorSectionComponent modulatorSection;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};

/*
  ==============================================================================

    GlobalSectionComponent.h
    Volume, polyphony, bend (left column); load/save, color theme, lowpass (right column) — 3:1 split.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include <functional>
#include <memory>

#include "ChoiceControl.h"
#include "HorizontalSliderControl.h"
#include "SectionLabel.h"
#include "ToggleSwitchControl.h"

class NewProjectAudioProcessor;

namespace MagicalFDS::UI
{
class GlobalSectionComponent final : public juce::Component
{
public:
    GlobalSectionComponent (NewProjectAudioProcessor& processor,
                            std::function<void()> onStateLoaded);

    void resized() override;

private:
    void showSaveDialog();
    void showLoadDialog();

    NewProjectAudioProcessor& audioProcessor;
    std::function<void()> onStateLoadedCallback;

    SectionLabel heading { "Global" };

    HorizontalSliderControl gain;
    HorizontalSliderControl polyphony;
    HorizontalSliderControl bendRange;
    juce::TextButton      loadButton { "Load" };
    juce::TextButton      saveButton { "Save" };
    ChoiceControl         colorTheme;
    ToggleSwitchControl   lowpass;

    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlobalSectionComponent)
};
} // namespace MagicalFDS::UI

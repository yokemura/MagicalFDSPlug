/*
  ==============================================================================

    ToggleSwitchControl.h
    Label + ToggleButton (Magical8bitPlug2 CheckBoxComponent-like).

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace MagicalFDS::UI
{
class ToggleSwitchControl final : public juce::Component
{
public:
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    ToggleSwitchControl (juce::AudioProcessorValueTreeState& apvts,
                         const juce::String& paramId,
                         const juce::String& name);

    void resized() override;

    juce::ToggleButton& getButton() noexcept { return toggle; }
    juce::Label&        getLabel()  noexcept { return label; }

private:
    std::unique_ptr<ButtonAttachment> attachment;

    juce::Label label;
    juce::ToggleButton toggle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToggleSwitchControl)
};
} // namespace MagicalFDS::UI


/*
  ==============================================================================

    ChoiceControl.h
    Label + ComboBox (Magical8bitPlug2-like).

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace MagicalFDS::UI
{
class ChoiceControl final : public juce::Component
{
public:
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    ChoiceControl (juce::AudioProcessorValueTreeState& apvts,
                   const juce::String& paramId,
                   const juce::String& name);

    void resized() override;

    juce::ComboBox& getComboBox() noexcept { return comboBox; }
    juce::Label&    getLabel()    noexcept { return label; }

private:
    std::unique_ptr<ComboBoxAttachment> attachment;

    juce::ComboBox comboBox;
    juce::Label label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChoiceControl)
};
} // namespace MagicalFDS::UI


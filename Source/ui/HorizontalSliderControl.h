/*
  ==============================================================================

    HorizontalSliderControl.h
    Label + horizontal slider + textbox (Magical8bitPlug2-like).

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace MagicalFDS::UI
{
class HorizontalSliderControl final : public juce::Component
{
public:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    HorizontalSliderControl (juce::AudioProcessorValueTreeState& apvts,
                             const juce::String& paramId,
                             const juce::String& name);

    void resized() override;

    juce::Slider& getSlider() noexcept { return slider; }
    juce::Label&  getLabel()  noexcept { return label; }

private:
    std::unique_ptr<SliderAttachment> attachment;

    juce::Label label;
    juce::Slider slider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HorizontalSliderControl)
};
} // namespace MagicalFDS::UI


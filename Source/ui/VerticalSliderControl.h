/*
  ==============================================================================

    VerticalSliderControl.h
    Thin vertical slider with optional label (for drawbars / wave steps).

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace MagicalFDS::UI
{
class VerticalSliderControl final : public juce::Component
{
public:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    VerticalSliderControl (juce::AudioProcessorValueTreeState* apvtsOrNull,
                           const juce::String& paramIdOrEmpty,
                           const juce::String& caption);

    /** Connect this control to APVTS param (replaces existing attachment). */
    void attach (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramId);

    /** Disconnect from APVTS param. */
    void detach();

    void resized() override;

    juce::Slider& getSlider() noexcept { return slider; }
    juce::Label&  getLabel()  noexcept { return label; }

private:
    // SliderAttachment::~... が slider に触れるため、attachment は slider より後に破棄する。
    juce::Slider slider;
    juce::Label  label;
    std::unique_ptr<SliderAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VerticalSliderControl)
};
} // namespace MagicalFDS::UI


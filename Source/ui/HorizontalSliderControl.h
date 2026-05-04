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

    /** @param bipolarFromCentre 真のとき、値 0 をトラック中央とみなし左右へ塗り分け（Morph 用）。 */
    HorizontalSliderControl (juce::AudioProcessorValueTreeState& apvts,
                               const juce::String& paramId,
                               const juce::String& name,
                               bool bipolarFromCentre = false);

    void resized() override;

    juce::Slider& getSlider() noexcept { return slider; }
    juce::Label&  getLabel()  noexcept { return label; }

private:
    // SliderAttachment::~... が slider に触れるため、attachment は slider より後に破棄する。
    juce::Label label;
    juce::Slider slider;
    std::unique_ptr<SliderAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HorizontalSliderControl)
};
} // namespace MagicalFDS::UI


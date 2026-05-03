/*
  ==============================================================================

    CarrierControlPanel.h
    Carrier waveform control area — fixed height; content swaps by carrier mode.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ChoiceControl.h"
#include "HorizontalSliderControl.h"
#include "VerticalSliderControl.h"

namespace MagicalFDS::UI
{
/** 仕様: コントロールパートのサイズはモードに依らず固定。自由描画時は空表示。 */
class CarrierControlPanel final : public juce::Component
{
public:
    explicit CarrierControlPanel (juce::AudioProcessorValueTreeState& apvts);

    void setCarrierMode (int carrierModeIndex);
    void resized() override;

private:
    class AdditiveStrip final : public juce::Component
    {
    public:
        explicit AdditiveStrip (juce::AudioProcessorValueTreeState& apvts);
        void resized() override;

    private:
        std::array<std::unique_ptr<VerticalSliderControl>, 8> drawbars;
    };

    AdditiveStrip additiveStrip;
    ChoiceControl presetChoice;
    HorizontalSliderControl morphSlider;
    HorizontalSliderControl pulseWidth;

    juce::Component freeDrawPlaceholder;

    int currentMode = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CarrierControlPanel)
};
} // namespace MagicalFDS::UI

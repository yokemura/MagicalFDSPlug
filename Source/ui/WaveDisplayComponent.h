/*
  ==============================================================================

    WaveDisplayComponent.h
    32-step vertical sliders for carrier / modulator wavetable preview.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "../FDSPatch.h"
#include "../Parameters.h"

namespace MagicalFDS::UI
{
enum class WaveDisplayKind
{
    carrier,
    modulator
};

/** キャリアはモードに応じて自由描画（APVTS 接続）または読み取り専用同期。モジュレータは常に読み取り専用。 */
class WaveDisplayComponent final : public juce::Component,
                                   private juce::Timer
{
public:
    WaveDisplayComponent (juce::AudioProcessorValueTreeState& apvtsIn, WaveDisplayKind kindIn);
    ~WaveDisplayComponent() override;

    void setCarrierFreeDrawEnabled (bool enabled);

    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    void timerCallback() override;
    void syncFromPatch();
    void updateAttachmentsAndTimer();

    juce::AudioProcessorValueTreeState& apvts;
    WaveDisplayKind kind;

    std::array<std::unique_ptr<juce::Slider>, FDSPatch::waveSteps> sliders;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, FDSPatch::waveSteps>
        freeDrawAttachments;

    bool carrierFreeDraw = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveDisplayComponent)
};
} // namespace MagicalFDS::UI

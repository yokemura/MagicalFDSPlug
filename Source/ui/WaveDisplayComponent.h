/*
  ==============================================================================

    WaveDisplayComponent.h
    キャリア 64 本 / モジュレータ 32 本の縦スライダーで wavetable プレビュー。

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include <memory>
#include <vector>

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
    const int numSteps;

    std::vector<std::unique_ptr<juce::Slider>> sliders;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> freeDrawAttachments;

    bool carrierFreeDraw = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveDisplayComponent)
};
} // namespace MagicalFDS::UI

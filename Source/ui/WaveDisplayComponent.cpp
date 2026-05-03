/*
  ==============================================================================

    WaveDisplayComponent.cpp

  ==============================================================================
*/

#include "WaveDisplayComponent.h"

#include "../PatchBuilder.h"

namespace MagicalFDS::UI
{
WaveDisplayComponent::WaveDisplayComponent (juce::AudioProcessorValueTreeState& apvtsIn,
                                            WaveDisplayKind kindIn)
    : apvts (apvtsIn), kind (kindIn)
{
    for (int i = 0; i < FDSPatch::waveSteps; ++i)
    {
        auto s = std::make_unique<juce::Slider>();
        s->setSliderStyle (juce::Slider::LinearVertical);
        s->setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        s->setRange (0.0, 63.0, 1.0);
        s->setScrollWheelEnabled (false);
        addAndMakeVisible (*s);
        sliders[(size_t) i] = std::move (s);
    }

    updateAttachmentsAndTimer();
}

WaveDisplayComponent::~WaveDisplayComponent()
{
    stopTimer();
}

void WaveDisplayComponent::setCarrierFreeDrawEnabled (bool enabled)
{
    if (kind != WaveDisplayKind::carrier)
        return;

    carrierFreeDraw = enabled;
    updateAttachmentsAndTimer();
}

void WaveDisplayComponent::updateAttachmentsAndTimer()
{
    for (auto& a : freeDrawAttachments)
        a.reset();

    if (kind == WaveDisplayKind::carrier && carrierFreeDraw)
    {
        stopTimer();

        for (int i = 0; i < FDSPatch::waveSteps; ++i)
        {
            auto& s = *sliders[(size_t) i];
            s.setEnabled (true);
            s.setInterceptsMouseClicks (true, true);

            freeDrawAttachments[(size_t) i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, makeFreeDrawId (i), s);
        }
    }
    else
    {
        for (auto& s : sliders)
        {
            s->setEnabled (true);
            s->setInterceptsMouseClicks (false, false);
        }

        syncFromPatch();

        if (kind == WaveDisplayKind::modulator
            || (kind == WaveDisplayKind::carrier && ! carrierFreeDraw))
            startTimerHz (15);
        else
            stopTimer();
    }
}

void WaveDisplayComponent::timerCallback()
{
    syncFromPatch();
}

void WaveDisplayComponent::syncFromPatch()
{
    FDSPatch patch {};
    applyApvtsToPatch (apvts, patch);

    for (int i = 0; i < FDSPatch::waveSteps; ++i)
    {
        auto& s = *sliders[(size_t) i];

        if (kind == WaveDisplayKind::carrier)
            s.setValue (patch.carrierWave[(size_t) i], juce::dontSendNotification);
        else
        {
            const double v = (double) (patch.modWave[(size_t) i] & 0x07) * (63.0 / 7.0);
            s.setValue (v, juce::dontSendNotification);
        }
    }
}

void WaveDisplayComponent::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat().reduced (0.5f);
    g.setColour (findColour (juce::Slider::backgroundColourId));
    g.fillRoundedRectangle (r, 4.0f);

    g.setColour (findColour (juce::Slider::textBoxOutlineColourId));
    g.drawRoundedRectangle (r, 4.0f, 1.0f);
}

void WaveDisplayComponent::resized()
{
    auto r = getLocalBounds().reduced (4, 4);
    const int n = FDSPatch::waveSteps;
    const int w = juce::jmax (1, r.getWidth() / n);

    for (int i = 0; i < n; ++i)
        sliders[(size_t) i]->setBounds (r.getX() + i * w, r.getY(), w, r.getHeight());
}
} // namespace MagicalFDS::UI

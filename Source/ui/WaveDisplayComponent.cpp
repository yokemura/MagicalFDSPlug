/*
  ==============================================================================

    WaveDisplayComponent.cpp

  ==============================================================================
*/

#include "WaveDisplayComponent.h"

#include "../ModWavePreview.h"
#include "../PatchBuilder.h"

namespace MagicalFDS::UI
{
WaveDisplayComponent::WaveDisplayComponent (juce::AudioProcessorValueTreeState& apvtsIn,
                                            WaveDisplayKind kindIn)
    : apvts (apvtsIn)
    , kind (kindIn)
    , numSteps (kindIn == WaveDisplayKind::carrier ? FDSPatch::carrierWaveSteps
                                                    : FDSPatch::modWaveSteps)
{
    freeDrawAttachments.resize ((size_t) numSteps);

    for (int i = 0; i < numSteps; ++i)
    {
        auto s = std::make_unique<juce::Slider>();
        s->setSliderStyle (juce::Slider::LinearVertical);
        s->setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        s->setRange (0.0, 63.0, 1.0);
        s->setScrollWheelEnabled (false);
        addAndMakeVisible (*s);
        sliders.push_back (std::move (s));
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

        for (int i = 0; i < numSteps; ++i)
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

    if (kind == WaveDisplayKind::carrier)
    {
        for (int i = 0; i < numSteps; ++i)
            sliders[(size_t) i]->setValue (patch.carrierWave[(size_t) i], juce::dontSendNotification);
    }
    else
    {
        std::array<uint8_t, FDSPatch::modWaveSteps> modPreview {};
        MagicalFDS::buildModWavePreviewLevels63 (patch.modWave, modPreview);

        for (int i = 0; i < numSteps; ++i)
            sliders[(size_t) i]->setValue (modPreview[(size_t) i], juce::dontSendNotification);
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
    const int w = juce::jmax (1, r.getWidth() / numSteps);

    for (int i = 0; i < numSteps; ++i)
        sliders[(size_t) i]->setBounds (r.getX() + i * w, r.getY(), w, r.getHeight());
}
} // namespace MagicalFDS::UI

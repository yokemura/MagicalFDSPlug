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
struct WaveDisplayComponent::CarrierFreeDrawOverlay final : public juce::Component
{
    explicit CarrierFreeDrawOverlay (juce::AudioProcessorValueTreeState& apvtsIn, int numStepsIn)
        : apvts (apvtsIn)
        , numSteps (numStepsIn)
    {
        jassert (numSteps > 0);
        gestureOpen.resize ((size_t) numSteps, false);
        setOpaque (false);
        setInterceptsMouseClicks (true, true);
    }

    ~CarrierFreeDrawOverlay() override
    {
        endStroke();
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (! e.mods.isLeftButtonDown())
            return;

        endStroke();
        lastIndex = -1;
        applyPoint (e.getPosition());
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        if (! e.mods.isLeftButtonDown())
            return;

        applyPoint (e.getPosition());
    }

    void mouseUp (const juce::MouseEvent&) override
    {
        endStroke();
        lastIndex = -1;
    }

private:
    void paint (juce::Graphics&) override {}

    int xToIndex (int x) const
    {
        const int w = juce::jmax (1, getWidth() / numSteps);
        return juce::jlimit (0, numSteps - 1, x / w);
    }

    int yToValue (int y) const
    {
        const int h = juce::jmax (1, getHeight());
        const int yc = juce::jlimit (0, h - 1, y);
        const double t = (double) (h - 1 - yc) / (double) juce::jmax (1, h - 1);
        return juce::jlimit (0, 63, juce::roundToInt (63.0 * t));
    }

    void setParamValue (int i, int v)
    {
        if (! juce::isPositiveAndBelow (i, numSteps))
            return;

        auto* p  = apvts.getParameter (makeFreeDrawId (i));
        auto* pi = dynamic_cast<juce::AudioParameterInt*> (p);
        if (pi == nullptr)
            return;

        const int c = juce::jlimit (0, 63, v);
        if (pi->get() == c)
            return;

        if (! gestureOpen[(size_t) i])
        {
            pi->beginChangeGesture();
            gestureOpen[(size_t) i] = true;
        }

        pi->setValueNotifyingHost (pi->getNormalisableRange().convertTo0to1 ((float) c));
    }

    void paintSegment (int i0, int v0, int i1, int v1)
    {
        if (i0 == i1)
        {
            setParamValue (i1, v1);
            return;
        }

        const int step = (i1 > i0) ? 1 : -1;
        for (int i = i0;; i += step)
        {
            const double t = (double) (i - i0) / (double) (i1 - i0);
            const int    vv = juce::roundToInt ((double) v0 + (double) (v1 - v0) * t);
            setParamValue (i, vv);
            if (i == i1)
                break;
        }
    }

    void applyPoint (juce::Point<int> pt)
    {
        if (getWidth() <= 0 || getHeight() <= 0)
            return;

        pt.setX (juce::jlimit (0, getWidth() - 1, pt.x));
        pt.setY (juce::jlimit (0, getHeight() - 1, pt.y));

        const int i = xToIndex (pt.x);
        const int v = yToValue (pt.y);

        if (lastIndex < 0)
        {
            setParamValue (i, v);
            lastIndex = i;
            lastValue = v;
            return;
        }

        paintSegment (lastIndex, lastValue, i, v);
        lastIndex = i;
        lastValue = v;
    }

    void endStroke()
    {
        for (int i = 0; i < numSteps; ++i)
        {
            if (! gestureOpen[(size_t) i])
                continue;

            if (auto* pi = dynamic_cast<juce::AudioParameterInt*> (apvts.getParameter (makeFreeDrawId (i))))
                pi->endChangeGesture();

            gestureOpen[(size_t) i] = false;
        }
    }

    juce::AudioProcessorValueTreeState& apvts;
    const int                           numSteps;

    std::vector<bool> gestureOpen;
    int               lastIndex = -1;
    int               lastValue = 0;
};

//==============================================================================
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

    if (kind == WaveDisplayKind::carrier)
    {
        carrierFreeDrawOverlay = std::make_unique<CarrierFreeDrawOverlay> (apvts, numSteps);
        addChildComponent (*carrierFreeDrawOverlay);
        carrierFreeDrawOverlay->setVisible (false);
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
            s.setInterceptsMouseClicks (false, false);

            freeDrawAttachments[(size_t) i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                apvts, makeFreeDrawId (i), s);
        }

        if (carrierFreeDrawOverlay != nullptr)
        {
            carrierFreeDrawOverlay->setVisible (true);
            carrierFreeDrawOverlay->toFront (false);
        }
    }
    else
    {
        if (carrierFreeDrawOverlay != nullptr)
            carrierFreeDrawOverlay->setVisible (false);

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

    if (carrierFreeDrawOverlay != nullptr)
        carrierFreeDrawOverlay->setBounds (r);
}
} // namespace MagicalFDS::UI

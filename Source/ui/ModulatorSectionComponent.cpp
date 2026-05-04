/*
  ==============================================================================

    ModulatorSectionComponent.cpp

  ==============================================================================
*/

#include "ModulatorSectionComponent.h"

#include "../LayoutConstants.h"
#include "../Parameters.h"

namespace MagicalFDS::UI
{
ModulatorSectionComponent::ModulatorSectionComponent (juce::AudioProcessorValueTreeState& apvtsIn)
    : apvts (apvtsIn)
    , waveDisplay (apvtsIn, WaveDisplayKind::modulator)
    , modRateUseControl (apvtsIn, ParamIDs::modRateUse, "Use")
    , modDepth (apvtsIn, ParamIDs::modDepth, "Mod depth")
    , modA (apvtsIn, ParamIDs::modA, "Attack")
    , modD (apvtsIn, ParamIDs::modD, "Decay")
    , modS (apvtsIn, ParamIDs::modS, "Sustain")
    , modR (apvtsIn, ParamIDs::modR, "Release")
{
    addAndMakeVisible (heading);
    addAndMakeVisible (waveDisplay);

    const int radioGroup = 90210;
    modWaveTri.setRadioGroupId (radioGroup);
    modWaveSaw.setRadioGroupId (radioGroup);
    modWaveTri.setClickingTogglesState (true);
    modWaveSaw.setClickingTogglesState (true);
    addAndMakeVisible (modWaveTri);
    addAndMakeVisible (modWaveSaw);

    modWaveTri.onClick = [this]
    {
        if (modWaveTri.getToggleState())
            setModWaveIndex (ParamChoices::ModWaveTriangle);
    };
    modWaveSaw.onClick = [this]
    {
        if (modWaveSaw.getToggleState())
            setModWaveIndex (ParamChoices::ModWaveSawtooth);
    };

    addAndMakeVisible (modRateUseControl);

    modRateLabel.setText ("Mod rate", juce::dontSendNotification);
    modRateLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (modRateLabel);

    modRateSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    modRateSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 80, Layout::rowHeight);
    modRateSlider.setScrollWheelEnabled (false);
    addAndMakeVisible (modRateSlider);

    modRateSlider.onValueChange = [this] { pushModRateSliderToParameter(); };

    addAndMakeVisible (modDepth);
    addAndMakeVisible (modA);
    addAndMakeVisible (modD);
    addAndMakeVisible (modS);
    addAndMakeVisible (modR);

    apvts.addParameterListener (ParamIDs::modWaveType, this);
    apvts.addParameterListener (ParamIDs::modRateUse, this);
    apvts.addParameterListener (ParamIDs::modRate, this);

    syncWaveButtonsFromParameter();
    clampModRateToModeRange();
    syncModRateSliderFromParameter();
}

ModulatorSectionComponent::~ModulatorSectionComponent()
{
    apvts.removeParameterListener (ParamIDs::modWaveType, this);
    apvts.removeParameterListener (ParamIDs::modRateUse, this);
    apvts.removeParameterListener (ParamIDs::modRate, this);
}

void ModulatorSectionComponent::parameterChanged (const juce::String& parameterID, float newValue)
{
    juce::ignoreUnused (newValue);

    if (parameterID == ParamIDs::modWaveType)
        syncWaveButtonsFromParameter();
    else if (parameterID == ParamIDs::modRateUse)
    {
        clampModRateToModeRange();
        syncModRateSliderFromParameter();
    }
    else if (parameterID == ParamIDs::modRate)
        syncModRateSliderFromParameter();
}

void ModulatorSectionComponent::clampModRateToModeRange()
{
    auto* useP  = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (ParamIDs::modRateUse));
    auto* rateP = dynamic_cast<juce::AudioParameterInt*> (apvts.getParameter (ParamIDs::modRate));
    if (useP == nullptr || rateP == nullptr)
        return;

    const int mode = useP->getIndex();
    if (mode == ParamChoices::ModRateOff)
        return;

    const int lo = (mode == ParamChoices::ModRateLFO) ? 1 : 30;
    const int hi = (mode == ParamChoices::ModRateLFO) ? 30 : 4095;
    const int r  = rateP->get();
    const int c  = juce::jlimit (lo, hi, r);
    if (c == r)
        return;

    rateP->beginChangeGesture();
    rateP->setValueNotifyingHost (rateP->getNormalisableRange().convertTo0to1 ((float) c));
    rateP->endChangeGesture();
}

void ModulatorSectionComponent::syncModRateSliderFromParameter()
{
    auto* useP  = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (ParamIDs::modRateUse));
    auto* rateP = dynamic_cast<juce::AudioParameterInt*> (apvts.getParameter (ParamIDs::modRate));
    if (useP == nullptr || rateP == nullptr)
        return;

    const int mode = useP->getIndex();
    if (mode == ParamChoices::ModRateOff)
    {
        modRateSlider.setVisible (false);
        return;
    }

    modRateSlider.setVisible (true);
    const int lo = (mode == ParamChoices::ModRateLFO) ? 1 : 30;
    const int hi = (mode == ParamChoices::ModRateLFO) ? 30 : 4095;
    modRateSlider.setRange ((double) lo, (double) hi, 1.0);
    const int v = juce::jlimit (lo, hi, rateP->get());
    modRateSlider.setValue ((double) v, juce::dontSendNotification);
}

void ModulatorSectionComponent::pushModRateSliderToParameter()
{
    if (! modRateSlider.isVisible())
        return;

    auto* rateP = dynamic_cast<juce::AudioParameterInt*> (apvts.getParameter (ParamIDs::modRate));
    if (rateP == nullptr)
        return;

    const int v = (int) std::lround (modRateSlider.getValue());
    if (rateP->get() == v)
        return;

    rateP->beginChangeGesture();
    rateP->setValueNotifyingHost (rateP->getNormalisableRange().convertTo0to1 ((float) v));
    rateP->endChangeGesture();
}

void ModulatorSectionComponent::setModWaveIndex (int index)
{
    if (auto* p = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (ParamIDs::modWaveType)))
    {
        const float nv = p->getNormalisableRange().convertTo0to1 ((float) index);
        p->beginChangeGesture();
        p->setValueNotifyingHost (nv);
        p->endChangeGesture();
    }
}

void ModulatorSectionComponent::syncWaveButtonsFromParameter()
{
    int idx = ParamChoices::ModWaveTriangle;

    if (auto* p = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (ParamIDs::modWaveType)))
        idx = p->getIndex();

    modWaveTri.setToggleState (idx == ParamChoices::ModWaveTriangle, juce::dontSendNotification);
    modWaveSaw.setToggleState (idx == ParamChoices::ModWaveSawtooth, juce::dontSendNotification);
}

void ModulatorSectionComponent::resized()
{
    auto r = getLocalBounds();

    heading.setBounds (r.removeFromTop (Layout::sectionHeaderHeight));
    r.removeFromTop (Layout::sectionHeadingContentGap);

    const int waveH = Layout::modWaveDisplayHeight;
    auto waveRow = r.removeFromTop (waveH);
    const int waveW = juce::jmin (Layout::modWaveDisplayWidth, waveRow.getWidth());
    waveDisplay.setBounds (waveRow.withSizeKeepingCentre (waveW, waveH));

    r.removeFromTop (Layout::componentMargin);

    auto waveBtnRow = r.removeFromTop (Layout::rowHeight);
    const int half = waveBtnRow.getWidth() / 2;
    modWaveTri.setBounds (waveBtnRow.removeFromLeft (half).reduced (2, 1));
    modWaveSaw.setBounds (waveBtnRow.reduced (2, 1));

    r.removeFromTop (Layout::componentMargin);

    modRateUseControl.setBounds (r.removeFromTop (Layout::rowHeight));

    r.removeFromTop (Layout::componentMargin);

    auto modRateRow = r.removeFromTop (Layout::rowHeight);
    {
        auto labelArea = modRateRow.removeFromLeft (Layout::labelColumnWidth);
        modRateLabel.setBounds (labelArea.reduced (0, 1));
        modRateRow.removeFromLeft (Layout::labelControlGap);
        modRateSlider.setBounds (modRateRow.reduced (0, 1));
    }

    r.removeFromTop (Layout::componentMargin);
    modDepth.setBounds (r.removeFromTop (Layout::rowHeight));

    r.removeFromTop (Layout::componentMargin);

    auto adsr = r.removeFromTop (Layout::adsrBlockHeight);
    modA.setBounds (adsr.removeFromTop (Layout::rowHeight));
    modD.setBounds (adsr.removeFromTop (Layout::rowHeight));
    modS.setBounds (adsr.removeFromTop (Layout::rowHeight));
    modR.setBounds (adsr.removeFromTop (Layout::rowHeight));
}
} // namespace MagicalFDS::UI

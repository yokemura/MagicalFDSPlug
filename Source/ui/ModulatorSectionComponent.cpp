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

    modDepthLabel.setText ("Mod depth", juce::dontSendNotification);
    modDepthLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (modDepthLabel);

    modDepthSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    modDepthSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 80, Layout::rowHeight);
    modDepthSlider.setScrollWheelEnabled (false);
    addAndMakeVisible (modDepthSlider);

    modDepthSlider.onValueChange = [this] { pushModDepthSliderToParameter(); };

    addAndMakeVisible (modA);
    addAndMakeVisible (modD);
    addAndMakeVisible (modS);
    addAndMakeVisible (modR);

    apvts.addParameterListener (ParamIDs::modWaveType, this);
    apvts.addParameterListener (ParamIDs::modRateUse, this);
    apvts.addParameterListener (ParamIDs::modRate, this);
    apvts.addParameterListener (ParamIDs::modDepth, this);

    syncWaveButtonsFromParameter();
    clampModRateToModeRange();
    syncModRateSliderFromParameter();
    clampModDepthToModeRange();
    syncModDepthSliderFromParameter();
}

ModulatorSectionComponent::~ModulatorSectionComponent()
{
    apvts.removeParameterListener (ParamIDs::modWaveType, this);
    apvts.removeParameterListener (ParamIDs::modRateUse, this);
    apvts.removeParameterListener (ParamIDs::modRate, this);
    apvts.removeParameterListener (ParamIDs::modDepth, this);
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
        clampModDepthToModeRange();
        syncModDepthSliderFromParameter();
    }
    else if (parameterID == ParamIDs::modRate)
        syncModRateSliderFromParameter();
    else if (parameterID == ParamIDs::modDepth)
        syncModDepthSliderFromParameter();
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

void ModulatorSectionComponent::clampModDepthToModeRange()
{
    auto* useP   = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (ParamIDs::modRateUse));
    auto* depthP = dynamic_cast<juce::AudioParameterInt*> (apvts.getParameter (ParamIDs::modDepth));
    if (useP == nullptr || depthP == nullptr)
        return;

    const int mode = useP->getIndex();
    if (mode == ParamChoices::ModRateOff)
        return;

    int lo = 0;
    int hi = 63;
    if (mode == ParamChoices::ModRateLFO)
    {
        lo = 1;
        hi = 5;
    }

    const int d = depthP->get();
    const int c = juce::jlimit (lo, hi, d);
    if (c == d)
        return;

    depthP->beginChangeGesture();
    depthP->setValueNotifyingHost (depthP->getNormalisableRange().convertTo0to1 ((float) c));
    depthP->endChangeGesture();
}

void ModulatorSectionComponent::syncModDepthSliderFromParameter()
{
    auto* useP   = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (ParamIDs::modRateUse));
    auto* depthP = dynamic_cast<juce::AudioParameterInt*> (apvts.getParameter (ParamIDs::modDepth));
    if (useP == nullptr || depthP == nullptr)
        return;

    const int mode = useP->getIndex();
    if (mode == ParamChoices::ModRateOff)
    {
        modDepthLabel.setVisible (false);
        modDepthSlider.setVisible (false);
        return;
    }

    modDepthLabel.setVisible (true);
    modDepthSlider.setVisible (true);

    int lo = 0;
    int hi = 63;
    if (mode == ParamChoices::ModRateLFO)
    {
        lo = 1;
        hi = 5;
    }

    modDepthSlider.setRange ((double) lo, (double) hi, 1.0);
    const int v = juce::jlimit (lo, hi, depthP->get());
    modDepthSlider.setValue ((double) v, juce::dontSendNotification);
}

void ModulatorSectionComponent::pushModDepthSliderToParameter()
{
    if (! modDepthSlider.isVisible())
        return;

    auto* depthP = dynamic_cast<juce::AudioParameterInt*> (apvts.getParameter (ParamIDs::modDepth));
    if (depthP == nullptr)
        return;

    const int v = (int) std::lround (modDepthSlider.getValue());
    if (depthP->get() == v)
        return;

    depthP->beginChangeGesture();
    depthP->setValueNotifyingHost (depthP->getNormalisableRange().convertTo0to1 ((float) v));
    depthP->endChangeGesture();
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

    auto modDepthRow = r.removeFromTop (Layout::rowHeight);
    {
        auto labelArea = modDepthRow.removeFromLeft (Layout::labelColumnWidth);
        modDepthLabel.setBounds (labelArea.reduced (0, 1));
        modDepthRow.removeFromLeft (Layout::labelControlGap);
        modDepthSlider.setBounds (modDepthRow.reduced (0, 1));
    }

    r.removeFromTop (Layout::componentMargin);

    auto adsr = r.removeFromTop (Layout::adsrBlockHeight);
    modA.setBounds (adsr.removeFromTop (Layout::rowHeight));
    modD.setBounds (adsr.removeFromTop (Layout::rowHeight));
    modS.setBounds (adsr.removeFromTop (Layout::rowHeight));
    modR.setBounds (adsr.removeFromTop (Layout::rowHeight));
}
} // namespace MagicalFDS::UI

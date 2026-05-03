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
    , modRate (apvtsIn, ParamIDs::modRate, "Mod rate")
    , modDepth (apvtsIn, ParamIDs::modDepth, "Mod depth")
    , modA (apvtsIn, ParamIDs::modA, "A")
    , modD (apvtsIn, ParamIDs::modD, "D")
    , modS (apvtsIn, ParamIDs::modS, "S")
    , modR (apvtsIn, ParamIDs::modR, "R")
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

    addAndMakeVisible (modRate);
    addAndMakeVisible (modDepth);
    addAndMakeVisible (modA);
    addAndMakeVisible (modD);
    addAndMakeVisible (modS);
    addAndMakeVisible (modR);

    apvts.addParameterListener (ParamIDs::modWaveType, this);

    syncWaveButtonsFromParameter();
}

ModulatorSectionComponent::~ModulatorSectionComponent()
{
    apvts.removeParameterListener (ParamIDs::modWaveType, this);
}

void ModulatorSectionComponent::parameterChanged (const juce::String& parameterID, float newValue)
{
    juce::ignoreUnused (newValue);

    if (parameterID == ParamIDs::modWaveType)
        syncWaveButtonsFromParameter();
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
    modRate.setBounds (r.removeFromTop (Layout::rowHeight));
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

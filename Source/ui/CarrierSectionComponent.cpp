/*
  ==============================================================================

    CarrierSectionComponent.cpp

  ==============================================================================
*/

#include "CarrierSectionComponent.h"

#include "../LayoutConstants.h"
#include "../Parameters.h"

namespace MagicalFDS::UI
{
CarrierSectionComponent::CarrierSectionComponent (juce::AudioProcessorValueTreeState& apvtsIn)
    : apvts (apvtsIn)
    , carrierA (apvtsIn, ParamIDs::carrierA, "A")
    , carrierD (apvtsIn, ParamIDs::carrierD, "D")
    , carrierS (apvtsIn, ParamIDs::carrierS, "S")
    , carrierR (apvtsIn, ParamIDs::carrierR, "R")
    , carrierMode (apvtsIn, ParamIDs::carrierMode, "Wave mode")
    , waveDisplay (apvtsIn, WaveDisplayKind::carrier)
    , controlPanel (apvtsIn)
{
    addAndMakeVisible (heading);
    addAndMakeVisible (carrierA);
    addAndMakeVisible (carrierD);
    addAndMakeVisible (carrierS);
    addAndMakeVisible (carrierR);
    addAndMakeVisible (carrierMode);
    addAndMakeVisible (waveDisplay);
    addAndMakeVisible (controlPanel);

    carrierMode.getComboBox().onChange = [this] { syncModeFromParameter(); };

    syncModeFromParameter();
}

void CarrierSectionComponent::syncModeFromParameter()
{
    int idx = ParamChoices::ModeAdditive;

    if (auto* p = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (ParamIDs::carrierMode)))
        idx = p->getIndex();

    controlPanel.setCarrierMode (idx);
    waveDisplay.setCarrierFreeDrawEnabled (idx == ParamChoices::ModeFreeDraw);
}

void CarrierSectionComponent::resized()
{
    auto r = getLocalBounds();

    heading.setBounds (r.removeFromTop (Layout::sectionHeaderHeight));

    auto adsr = r.removeFromTop (Layout::adsrBlockHeight);
    carrierA.setBounds (adsr.removeFromTop (Layout::rowHeight));
    carrierD.setBounds (adsr.removeFromTop (Layout::rowHeight));
    carrierS.setBounds (adsr.removeFromTop (Layout::rowHeight));
    carrierR.setBounds (adsr.removeFromTop (Layout::rowHeight));

    r.removeFromTop (Layout::componentMargin);
    carrierMode.setBounds (r.removeFromTop (Layout::rowHeight));
    r.removeFromTop (Layout::componentMargin);

    const int waveH = Layout::carrierWaveDisplayHeight;
    auto waveRow = r.removeFromTop (waveH);
    const int waveW = juce::jmin (Layout::carrierWaveDisplayWidth, waveRow.getWidth());
    waveDisplay.setBounds (waveRow.withSizeKeepingCentre (waveW, waveH));

    r.removeFromTop (Layout::componentMargin);
    controlPanel.setBounds (r.removeFromTop (Layout::carrierControlPanelHeight));
}
} // namespace MagicalFDS::UI

/*
  ==============================================================================

    GlobalSectionComponent.cpp

  ==============================================================================
*/

#include "GlobalSectionComponent.h"

#include "../LayoutConstants.h"
#include "../Parameters.h"

namespace MagicalFDS::UI
{
GlobalSectionComponent::GlobalSectionComponent (juce::AudioProcessorValueTreeState& apvts)
    : gain (apvts, ParamIDs::gain, "Gain")
    , polyphony (apvts, ParamIDs::polyphony, "Polyphony")
    , bendRange (apvts, ParamIDs::bendRange, "Bend")
    , colorTheme (apvts, ParamIDs::colorTheme, "Theme")
    , lowpass (apvts, ParamIDs::lowpassEnabled, "LPF")
{
    addAndMakeVisible (heading);
    addAndMakeVisible (gain);
    addAndMakeVisible (polyphony);
    addAndMakeVisible (bendRange);
    addAndMakeVisible (colorTheme);
    addAndMakeVisible (lowpass);
}

void GlobalSectionComponent::resized()
{
    auto r = getLocalBounds();

    heading.setBounds (r.removeFromTop (Layout::sectionHeaderHeight));

    auto row1 = r.removeFromTop (Layout::rowHeight);
    auto row2 = r.removeFromTop (Layout::rowHeight);

    const int w1 = row1.getWidth() / 3;
    gain.setBounds (row1.removeFromLeft (w1));
    polyphony.setBounds (row1.removeFromLeft (w1));
    bendRange.setBounds (row1);

    const int w2 = row2.getWidth() / 2;
    colorTheme.setBounds (row2.removeFromLeft (w2));
    lowpass.setBounds (row2);
}
} // namespace MagicalFDS::UI

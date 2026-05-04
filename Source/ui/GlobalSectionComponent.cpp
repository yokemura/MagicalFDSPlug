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
    r.removeFromTop (Layout::sectionHeadingContentGap);

    const int leftW = (r.getWidth() * 3) / 4;
    auto leftCol = r.removeFromLeft (leftW);
    r.removeFromLeft (Layout::globalSectionColumnMargin);

    gain.setBounds (leftCol.removeFromTop (Layout::rowHeight));
    polyphony.setBounds (leftCol.removeFromTop (Layout::rowHeight));
    bendRange.setBounds (leftCol.removeFromTop (Layout::rowHeight));

    colorTheme.setBounds (r.removeFromTop (Layout::rowHeight));
    lowpass.setBounds (r.removeFromTop (Layout::rowHeight));
}
} // namespace MagicalFDS::UI

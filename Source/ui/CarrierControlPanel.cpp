/*
  ==============================================================================

    CarrierControlPanel.cpp

  ==============================================================================
*/

#include "CarrierControlPanel.h"

#include "../LayoutConstants.h"
#include "../Parameters.h"

namespace MagicalFDS::UI
{
//==============================================================================
CarrierControlPanel::AdditiveStrip::AdditiveStrip (juce::AudioProcessorValueTreeState& apvts)
{
    static const std::array<const char*, 8> labels {
        "8'", "4'", "2 2/3'", "2'", "1 3/5'", "1 1/3'", "1 1/7'", "1'"
    };

    for (int i = 0; i < 8; ++i)
    {
        drawbars[(size_t) i] = std::make_unique<VerticalSliderControl> (&apvts,
                                                                        makeDrawbarId (i),
                                                                        labels[(size_t) i]);
        addAndMakeVisible (*drawbars[(size_t) i]);
    }
}

void CarrierControlPanel::AdditiveStrip::resized()
{
    auto r = getLocalBounds();
    const int w = juce::jmax (1, r.getWidth() / 8);

    for (int i = 0; i < 8; ++i)
        drawbars[(size_t) i]->setBounds (r.getX() + i * w, r.getY(), w, r.getHeight());
}

//==============================================================================
CarrierControlPanel::CarrierControlPanel (juce::AudioProcessorValueTreeState& apvts)
    : additiveStrip (apvts)
    , presetChoice (apvts, ParamIDs::carrierPreset, "Preset")
    , morphSlider (apvts, ParamIDs::carrierMorphAmount, "Morph")
    , pulseWidth (apvts, ParamIDs::carrierPulseWidth, "Pulse width")
{
    addAndMakeVisible (additiveStrip);
    addChildComponent (presetChoice);
    addChildComponent (morphSlider);
    addChildComponent (pulseWidth);
    addAndMakeVisible (freeDrawPlaceholder);

    freeDrawPlaceholder.setInterceptsMouseClicks (false, false);

    setCarrierMode (ParamChoices::ModeAdditive);
}

void CarrierControlPanel::setCarrierMode (int carrierModeIndex)
{
    if (currentMode == carrierModeIndex)
        return;

    currentMode = carrierModeIndex;

    additiveStrip.setVisible (carrierModeIndex == ParamChoices::ModeAdditive);
    const bool preset = (carrierModeIndex == ParamChoices::ModePresetMorph);
    presetChoice.setVisible (preset);
    morphSlider.setVisible (preset);
    pulseWidth.setVisible (carrierModeIndex == ParamChoices::ModePulseShape);
    freeDrawPlaceholder.setVisible (carrierModeIndex == ParamChoices::ModeFreeDraw);
}

void CarrierControlPanel::resized()
{
    auto r = getLocalBounds();
    additiveStrip.setBounds (r);

    auto morphArea = r;
    presetChoice.setBounds (morphArea.removeFromTop (Layout::rowHeight));
    morphSlider.setBounds (morphArea.removeFromTop (Layout::rowHeight));

    pulseWidth.setBounds (r);
    freeDrawPlaceholder.setBounds (r);
}
} // namespace MagicalFDS::UI

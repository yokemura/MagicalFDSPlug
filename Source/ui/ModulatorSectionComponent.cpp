/*
  ==============================================================================

    ModulatorSectionComponent.cpp

  ==============================================================================
*/

#include "ModulatorSectionComponent.h"

#include "../ColorScheme.h"
#include "../LayoutConstants.h"
#include "../ModOpcodeText.h"
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
    modWaveSine.setRadioGroupId (radioGroup);
    modWaveSquare.setRadioGroupId (radioGroup);
    modWaveRise.setRadioGroupId (radioGroup);
    modWaveFall.setRadioGroupId (radioGroup);
    modWaveCustom.setRadioGroupId (radioGroup);

    for (auto* b : { &modWaveTri, &modWaveSaw, &modWaveSine, &modWaveSquare, &modWaveRise, &modWaveFall, &modWaveCustom })
    {
        b->setClickingTogglesState (true);
        addAndMakeVisible (*b);
    }

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
    modWaveSine.onClick = [this]
    {
        if (modWaveSine.getToggleState())
            setModWaveIndex (ParamChoices::ModWaveSine);
    };
    modWaveSquare.onClick = [this]
    {
        if (modWaveSquare.getToggleState())
            setModWaveIndex (ParamChoices::ModWaveSquare);
    };
    modWaveRise.onClick = [this]
    {
        if (modWaveRise.getToggleState())
            setModWaveIndex (ParamChoices::ModWaveOneShotUp);
    };
    modWaveFall.onClick = [this]
    {
        if (modWaveFall.getToggleState())
            setModWaveIndex (ParamChoices::ModWaveOneShotDown);
    };
    modWaveCustom.onClick = [this]
    {
        if (modWaveCustom.getToggleState())
            setModWaveIndex (ParamChoices::ModWaveCustom);
    };

    modOpcodeEditor.setMultiLine (false);
    modOpcodeEditor.setReturnKeyStartsNewLine (false);
    modOpcodeEditor.setScrollbarsShown (false);
    modOpcodeEditor.setFont (juce::FontOptions (14.f));
    modOpcodeEditor.onTextChange = [this] { handleOpcodeEditorTextChanged(); };
    addChildComponent (modOpcodeEditor);

    modOpcodeErrorLabel.setJustificationType (juce::Justification::centredLeft);
    addChildComponent (modOpcodeErrorLabel);

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
    syncCustomUiVisibility();
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

void ModulatorSectionComponent::setOnPreferredHeightChanged (std::function<void()> callback)
{
    onPreferredHeightChanged = std::move (callback);
}

int ModulatorSectionComponent::getPreferredHeight() const
{
    int h = Layout::modColumnBaseHeight;

    if (isCustomWaveSelected())
        h += Layout::modColumnCustomExtraHeight;

    return h;
}

bool ModulatorSectionComponent::isCustomWaveSelected() const
{
    if (auto* p = dynamic_cast<juce::AudioParameterChoice*> (
            apvts.getParameter (ParamIDs::modWaveType)))
        return p->getIndex() == ParamChoices::ModWaveCustom;

    return false;
}

void ModulatorSectionComponent::notifyPreferredHeightChanged()
{
    if (onPreferredHeightChanged)
        onPreferredHeightChanged();
}

void ModulatorSectionComponent::parameterChanged (const juce::String& parameterID, float newValue)
{
    juce::ignoreUnused (newValue);

    if (parameterID == ParamIDs::modWaveType)
    {
        syncWaveButtonsFromParameter();
        syncCustomUiVisibility();
    }
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

void ModulatorSectionComponent::saveDraftFromEditor()
{
    modOpcodeDraftText = modOpcodeEditor.getText();
}

void ModulatorSectionComponent::populateOpcodeEditorText()
{
    juce::String text;

    if (modOpcodeDraftText.isNotEmpty())
        text = modOpcodeDraftText;
    else if (isModCustomOpcodesActive (apvts))
        text = formatModOpcodeText (readModOpcodesFromApvts (apvts));

    modOpcodeEditorSyncInProgress = true;
    modOpcodeEditor.setText (text, juce::dontSendNotification);
    modOpcodeEditorSyncInProgress = false;

    handleOpcodeEditorTextChanged();
}

void ModulatorSectionComponent::handleOpcodeEditorTextChanged()
{
    if (modOpcodeEditorSyncInProgress)
        return;

    modOpcodeDraftText = modOpcodeEditor.getText();

    const auto parsed = parseModOpcodeText (modOpcodeEditor.getText());

    if (! parsed.ok)
    {
        modOpcodeErrorLabel.setText (parsed.error, juce::dontSendNotification);
        modOpcodeErrorLabel.setColour (juce::Label::textColourId,
                                       findColour (MagicalFDSColours::warningTextColourId));
        modOpcodeErrorLabel.setVisible (isCustomWaveSelected() && parsed.error.isNotEmpty());
        return;
    }

    modOpcodeErrorLabel.setVisible (false);
    commitValidOpcodes (parsed.opcodes);
}

void ModulatorSectionComponent::commitValidOpcodes (
    const std::array<uint8_t, FDSPatch::modWaveSteps>& opcodes)
{
    const auto current = readModOpcodesFromApvts (apvts);
    if (current == opcodes && isModCustomOpcodesActive (apvts))
        return;

    writeModOpcodesToApvts (apvts, opcodes);
    setModCustomOpcodesActive (apvts, true);
}

void ModulatorSectionComponent::syncCustomUiVisibility()
{
    const bool custom = isCustomWaveSelected();

    if (! custom)
        saveDraftFromEditor();

    modOpcodeEditor.setVisible (custom);
    modOpcodeErrorLabel.setVisible (custom && modOpcodeErrorLabel.getText().isNotEmpty());

    if (custom)
        populateOpcodeEditorText();

    notifyPreferredHeightChanged();
    resized();
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

void ModulatorSectionComponent::setModLastPresetIndex (int index)
{
    if (auto* p = dynamic_cast<juce::AudioParameterChoice*> (
            apvts.getParameter (ParamIDs::modLastPresetWaveType)))
    {
        const int clamped = juce::jlimit (0, (int) ParamChoices::ModWaveOneShotDown, index);
        if (p->getIndex() == clamped)
            return;

        p->beginChangeGesture();
        p->setValueNotifyingHost (p->getNormalisableRange().convertTo0to1 ((float) clamped));
        p->endChangeGesture();
    }
}

void ModulatorSectionComponent::setModWaveIndex (int index)
{
    const bool leavingCustom = isCustomWaveSelected() && index != ParamChoices::ModWaveCustom;
    if (leavingCustom)
        saveDraftFromEditor();

    if (index >= ParamChoices::ModWaveTriangle && index <= ParamChoices::ModWaveOneShotDown)
        setModLastPresetIndex (index);

    if (auto* p = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (ParamIDs::modWaveType)))
    {
        const float nv = p->getNormalisableRange().convertTo0to1 ((float) index);
        p->beginChangeGesture();
        p->setValueNotifyingHost (nv);
        p->endChangeGesture();
    }

    syncWaveButtonsFromParameter();
    syncCustomUiVisibility();
}

void ModulatorSectionComponent::syncWaveButtonsFromParameter()
{
    int idx = ParamChoices::ModWaveTriangle;

    if (auto* p = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (ParamIDs::modWaveType)))
        idx = p->getIndex();

    modWaveTri.setToggleState    (idx == ParamChoices::ModWaveTriangle,     juce::dontSendNotification);
    modWaveSaw.setToggleState    (idx == ParamChoices::ModWaveSawtooth,     juce::dontSendNotification);
    modWaveSine.setToggleState   (idx == ParamChoices::ModWaveSine,         juce::dontSendNotification);
    modWaveSquare.setToggleState (idx == ParamChoices::ModWaveSquare,       juce::dontSendNotification);
    modWaveRise.setToggleState   (idx == ParamChoices::ModWaveOneShotUp,    juce::dontSendNotification);
    modWaveFall.setToggleState   (idx == ParamChoices::ModWaveOneShotDown,  juce::dontSendNotification);
    modWaveCustom.setToggleState (idx == ParamChoices::ModWaveCustom,       juce::dontSendNotification);
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

    {
        auto row = r.removeFromTop (Layout::rowHeight);
        const int third = row.getWidth() / 3;
        modWaveTri.setBounds    (row.removeFromLeft (third).reduced (2, 1));
        modWaveSaw.setBounds    (row.removeFromLeft (third).reduced (2, 1));
        modWaveSine.setBounds   (row.reduced (2, 1));
    }

    r.removeFromTop (Layout::componentMargin);

    {
        auto row = r.removeFromTop (Layout::rowHeight);
        const int third = row.getWidth() / 3;
        modWaveSquare.setBounds (row.removeFromLeft (third).reduced (2, 1));
        modWaveRise.setBounds   (row.removeFromLeft (third).reduced (2, 1));
        modWaveFall.setBounds   (row.reduced (2, 1));
    }

    r.removeFromTop (Layout::componentMargin);

    {
        auto row = r.removeFromTop (Layout::rowHeight);
        modWaveCustom.setBounds (row.removeFromLeft (row.getWidth() / 3).reduced (2, 1));
    }

    if (isCustomWaveSelected())
    {
        r.removeFromTop (Layout::componentMargin);
        modOpcodeEditor.setBounds (r.removeFromTop (Layout::modOpcodeEditorHeight).reduced (0, 1));

        r.removeFromTop (Layout::componentMargin);
        modOpcodeErrorLabel.setBounds (r.removeFromTop (Layout::modOpcodeErrorHeight));
    }

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

/*
  ==============================================================================

    ModulatorSectionComponent.h
    Modulator wave preview, wave type (triangle / saw / custom opcodes), rate, depth, ADSR.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include <functional>

#include "../FDSPatch.h"
#include "ChoiceControl.h"
#include "HorizontalSliderControl.h"
#include "SectionLabel.h"
#include "WaveDisplayComponent.h"

namespace MagicalFDS::UI
{
class ModulatorSectionComponent final : public juce::Component,
                                        private juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit ModulatorSectionComponent (juce::AudioProcessorValueTreeState& apvts);

    /** Custom UI 表示切替時にエディタ全体の高さを更新する。 */
    void setOnPreferredHeightChanged (std::function<void()> callback);

    int getPreferredHeight() const;

    ~ModulatorSectionComponent() override;

    void resized() override;

private:
    void parameterChanged (const juce::String& parameterID, float newValue) override;

    void syncWaveButtonsFromParameter();
    void syncCustomUiVisibility();
    void populateOpcodeEditorText();
    void handleOpcodeEditorTextChanged();
    void commitValidOpcodes (const std::array<uint8_t, FDSPatch::modWaveSteps>& opcodes);
    void saveDraftFromEditor();
    void setModWaveIndex (int index);
    void setModLastPresetIndex (int index);
    bool isCustomWaveSelected() const;

    void clampModRateToModeRange();
    void syncModRateSliderFromParameter();
    void pushModRateSliderToParameter();

    void clampModDepthToModeRange();
    void syncModDepthSliderFromParameter();
    void pushModDepthSliderToParameter();

    void notifyPreferredHeightChanged();

    juce::AudioProcessorValueTreeState& apvts;

    std::function<void()> onPreferredHeightChanged;

    SectionLabel heading { "Modulator" };

    WaveDisplayComponent waveDisplay;

    juce::ToggleButton modWaveTri    { "Triangle" };
    juce::ToggleButton modWaveSaw    { "Saw" };
    juce::ToggleButton modWaveSine   { "Sine" };
    juce::ToggleButton modWaveSquare { "Square" };
    juce::ToggleButton modWaveRise   { "Rise" };
    juce::ToggleButton modWaveFall   { "Fall" };
    juce::ToggleButton modWaveCustom { "Custom" };

    juce::TextEditor modOpcodeEditor;
    juce::Label      modOpcodeErrorLabel;

    /** Custom 離脱後もセッション内で復元するテキスト（APVTS 非保存）。 */
    juce::String modOpcodeDraftText;
    bool         modOpcodeEditorSyncInProgress = false;

    ChoiceControl modRateUseControl;
    juce::Label modRateLabel;
    juce::Slider modRateSlider;

    juce::Label modDepthLabel;
    juce::Slider modDepthSlider;

    HorizontalSliderControl modA;
    HorizontalSliderControl modD;
    HorizontalSliderControl modS;
    HorizontalSliderControl modR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModulatorSectionComponent)
};
} // namespace MagicalFDS::UI

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <array>

#include <JuceHeader.h>

#include "FDSPatch.h"
#include "Parameters.h"

//==============================================================================
/**
*/
class NewProjectAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    NewProjectAudioProcessor();
    ~NewProjectAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    /** UI から扱うパラメータツリー。エディタからは get/setParameterTree 経由で参照する。 */
    juce::AudioProcessorValueTreeState& getParameters() noexcept { return apvts; }

    /** APVTS 状態をプレーン XML ファイルへ書き出す（DAW 保存と同形式）。 */
    bool saveStateToXmlFile (const juce::File& file);

    /** プレーン XML ファイルから APVTS 状態を復元する。 */
    bool loadStateFromXmlFile (const juce::File& file);

private:
    //==============================================================================
    static constexpr int maxPolyphonyCap = 16;

    /** 現在構築済みのシンセボイス数（APVTS polyphony と一致）。 */
    int synthVoiceCount = 1;

    void syncPolyphonyFromApvts();

    static constexpr float kLpfCutoffHz = 2000.f;
    float lpAlpha = 0.f;
    std::array<float, 2> lpZ { 0.f, 0.f };

    void updateLpfCoefficient (double sampleRate);

    FDSPatch fdsPatch;

    juce::AudioProcessorValueTreeState apvts;

    juce::Synthesiser synth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessor)
};

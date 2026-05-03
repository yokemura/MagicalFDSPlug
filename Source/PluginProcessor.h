/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

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

private:
    //==============================================================================
    static constexpr int maxPolyphonyCap = 16;

    /** 現在構築済みのシンセボイス数（APVTS polyphony と一致）。 */
    int synthVoiceCount = 1;

    void syncPolyphonyFromApvts();

    FDSPatch fdsPatch;

    juce::AudioProcessorValueTreeState apvts;

    juce::Synthesiser synth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessor)
};

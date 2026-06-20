/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "FDSVoice.h"
#include "PatchBuilder.h"

#include <cmath>

namespace
{
    int readPolyphonyFromApvts (juce::AudioProcessorValueTreeState& apvts, int maxCap)
    {
        if (auto* p = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (MagicalFDS::ParamIDs::polyphony)))
            return juce::jlimit (1, maxCap, (int) std::lround (p->get()));

        return 1;
    }
}

//==============================================================================
NewProjectAudioProcessor::NewProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       apvts (*this, nullptr, juce::Identifier ("PARAMS"),
              MagicalFDS::createParameterLayout())
#else
     : apvts (*this, nullptr, juce::Identifier ("PARAMS"),
              MagicalFDS::createParameterLayout())
#endif
{
    fdsPatch.resetToDefaults();

    synth.setCurrentPlaybackSampleRate (44100.0);

    synthVoiceCount = readPolyphonyFromApvts (apvts, MagicalFDS::maxPolyphony);
    for (int i = 0; i < synthVoiceCount; ++i)
        synth.addVoice (new FDSVoice (&fdsPatch));

    synth.addSound (new FDSSound (&fdsPatch));

    MagicalFDS::applyApvtsToPatch (apvts, fdsPatch);
    MagicalFDS::applyRuntimeParametersFromApvts (apvts, fdsPatch);
}

void NewProjectAudioProcessor::syncPolyphonyFromApvts()
{
    const int poly = readPolyphonyFromApvts (apvts, MagicalFDS::maxPolyphony);
    if (poly == synthVoiceCount)
        return;

    synth.clearVoices();
    for (int i = 0; i < poly; ++i)
        synth.addVoice (new FDSVoice (&fdsPatch));

    synthVoiceCount = poly;
}

NewProjectAudioProcessor::~NewProjectAudioProcessor()
{
}

//==============================================================================
const juce::String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NewProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NewProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void NewProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    synth.setCurrentPlaybackSampleRate (sampleRate);
    updateLpfCoefficient (sampleRate);
    lpZ.fill (0.f);
}

void NewProjectAudioProcessor::updateLpfCoefficient (double sampleRate)
{
    if (sampleRate <= 0.0)
    {
        lpAlpha = 0.f;
        return;
    }

    lpAlpha = 1.0f - std::exp (-juce::MathConstants<float>::twoPi * kLpfCutoffHz / (float) sampleRate);
}

void NewProjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    MagicalFDS::applyApvtsToPatch (apvts, fdsPatch);
    MagicalFDS::applyRuntimeParametersFromApvts (apvts, fdsPatch);
    syncPolyphonyFromApvts();

    buffer.clear();
    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());

    if (fdsPatch.lowpassEnabled)
    {
        const int numCh = juce::jmin (buffer.getNumChannels(), (int) lpZ.size());
        const int n = buffer.getNumSamples();

        for (int ch = 0; ch < numCh; ++ch)
        {
            auto* d = buffer.getWritePointer (ch);
            float z = lpZ[(size_t) ch];

            for (int i = 0; i < n; ++i)
            {
                z += lpAlpha * (d[i] - z);
                d[i] = z;
            }

            lpZ[(size_t) ch] = z;
        }
    }
}

//==============================================================================
bool NewProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor (*this);
}

//==============================================================================
bool NewProjectAudioProcessor::saveStateToXmlFile (const juce::File& file)
{
    if (auto xml = apvts.copyState().createXml())
        return xml->writeTo (file);

    return false;
}

bool NewProjectAudioProcessor::loadStateFromXmlFile (const juce::File& file)
{
    if (auto xml = juce::parseXML (file))
    {
        if (xml->hasTagName (apvts.state.getType()))
        {
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
            syncPolyphonyFromApvts();
            return true;
        }
    }

    return false;
}

void NewProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}

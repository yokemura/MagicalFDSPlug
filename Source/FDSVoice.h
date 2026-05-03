/*
  ==============================================================================

    FDS-style wavetable voice (sample-rate aggregated DSP, shared FDSPatch).

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "FDSPatch.h"

//==============================================================================
class FDSVoice final : public juce::SynthesiserVoice
{
public:
    explicit FDSVoice (FDSPatch* sharedPatch);

    bool canPlaySound (juce::SynthesiserSound*) override;

    void startNote (int midiNoteNumber,
                    float velocity,
                    juce::SynthesiserSound*,
                    int currentPitchWheelPosition) override;

    void stopNote (float velocity, bool allowTailOff) override;

    void pitchWheelMoved (int newPitchWheelValue) override;
    void controllerMoved (int controllerNumber, int newValue) override;

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                          int startSample,
                          int numSamples) override;

private:
    FDSPatch* patch = nullptr;

    juce::ADSR carrierAdsr;
    juce::ADSR modAdsr;
    double adsrSampleRate = 0.0;

    float carrierPhase = 0.f;
    float modPhase = 0.f;
    int modCounter = 0;

    float velocity = 0.f;
    float pitchWheelRatio = 1.f;
    int currentPitchWheel = 8192;
    int currentMidiNote = 60;

    double carrierHz = 440.0;
    double modPhaseInc = 0.0;

    static constexpr float masterGain = 0.2f;

    float pitchWheelToRatio (int wheel14) const;

    void ensureAdsrSampleRate();
    void updateRatesFromPatch();
    float renderOneSample();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FDSVoice)
};

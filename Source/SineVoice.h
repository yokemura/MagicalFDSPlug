/*
  ==============================================================================

    Simple sine oscillator voice for MagicalFDSPlug.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class SineVoice final : public juce::SynthesiserVoice
{
public:
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
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    float level = 0.0f;

    static constexpr float masterGain = 0.15f;
};

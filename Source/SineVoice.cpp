/*
  ==============================================================================

    Simple sine oscillator voice for MagicalFDSPlug.

  ==============================================================================
*/

#include "SineVoice.h"

#include <cmath>

//==============================================================================
bool SineVoice::canPlaySound (juce::SynthesiserSound*)
{
    return true;
}

void SineVoice::startNote (int midiNoteNumber,
                           float velocity,
                           juce::SynthesiserSound*,
                           int /*currentPitchWheelPosition*/)
{
    currentAngle = 0.0;

    const auto cyclesPerSecond = (double) juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    angleDelta = juce::MathConstants<double>::twoPi * cyclesPerSecond / getSampleRate();

    level = masterGain * velocity;
}

void SineVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    if (! allowTailOff)
    {
        clearCurrentNote();
        angleDelta = 0.0;
        return;
    }

    clearCurrentNote();
    angleDelta = 0.0;
}

void SineVoice::pitchWheelMoved (int /*newPitchWheelValue*/) {}
void SineVoice::controllerMoved (int /*controllerNumber*/, int /*newValue*/) {}

void SineVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                                 int startSample,
                                 int numSamples)
{
    if (getCurrentlyPlayingNote() < 0 || angleDelta == 0.0)
        return;

    const auto numChannels = outputBuffer.getNumChannels();

    while (--numSamples >= 0)
    {
        const float s = level * (float) std::sin (currentAngle);

        for (int ch = 0; ch < numChannels; ++ch)
            outputBuffer.addSample (ch, startSample, s);

        currentAngle += angleDelta;

        if (currentAngle >= juce::MathConstants<double>::twoPi)
            currentAngle -= juce::MathConstants<double>::twoPi;

        ++startSample;
    }
}

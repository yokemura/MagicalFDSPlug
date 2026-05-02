/*
  ==============================================================================

    FDS-style wavetable voice (sample-rate aggregated DSP).

  ==============================================================================
*/

#include "FDSVoice.h"

#include <cmath>

//==============================================================================
FDSVoice::FDSVoice (FDSPatch* sharedPatch)
    : patch (sharedPatch)
{
    jassert (patch != nullptr);
}

bool FDSVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<FDSSound*> (sound) != nullptr;
}

void FDSVoice::startNote (int midiNoteNumber,
                          float vel,
                          juce::SynthesiserSound*,
                          int pitchWheelPosition)
{
    juce::ignoreUnused (pitchWheelPosition);

    currentMidiNote = midiNoteNumber;
    velocity = vel;
    currentPitchWheel = pitchWheelPosition;
    pitchWheelRatio = pitchWheelToRatio (currentPitchWheel);

    carrierPhase = 0.f;
    modPhase = 0.f;
    modCounter = 0;

    carrierHz = (double) juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber)
                * (double) pitchWheelRatio;

    if (patch != nullptr)
    {
        volumeEnvGain = (float) patch->getVolumeGainClamped();
        modEnvGain = (float) patch->getModGainClamped();
    }

    updateRatesFromPatch();
}

void FDSVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    juce::ignoreUnused (allowTailOff);
    clearCurrentNote();
}

void FDSVoice::pitchWheelMoved (int newPitchWheelValue)
{
    currentPitchWheel = newPitchWheelValue;
    pitchWheelRatio = pitchWheelToRatio (currentPitchWheel);
    carrierHz = (double) juce::MidiMessage::getMidiNoteInHertz (currentMidiNote)
                * (double) pitchWheelRatio;
}

void FDSVoice::controllerMoved (int /*controllerNumber*/, int /*newValue*/) {}

void FDSVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                                int startSample,
                                int numSamples)
{
    if (patch == nullptr || getCurrentlyPlayingNote() < 0)
        return;

    const auto numChannels = outputBuffer.getNumChannels();

    while (--numSamples >= 0)
    {
        const float s = renderOneSample();

        for (int ch = 0; ch < numChannels; ++ch)
            outputBuffer.addSample (ch, startSample, s);

        ++startSample;
    }
}

//==============================================================================
void FDSVoice::applyModWaveStep (uint8_t code, int& counter)
{
    switch (code & 7)
    {
        case 0:
            break;
        case 1:
            ++counter;
            break;
        case 2:
            counter += 2;
            break;
        case 3:
            counter += 4;
            break;
        case 4:
            counter = 0;
            break;
        case 5:
            counter -= 4;
            break;
        case 6:
            counter -= 2;
            break;
        case 7:
            --counter;
            break;
        default:
            break;
    }

    while (counter > 63)
        counter -= 128;
    while (counter < -64)
        counter += 128;
}

float FDSVoice::pitchWheelToRatio (int wheel14)
{
    const float x = ((float) wheel14 - 8192.f) / 8192.f;
    return std::pow (2.f, x * pitchBendSemis / 12.f);
}

void FDSVoice::updateRatesFromPatch()
{
    if (patch == nullptr)
        return;

    const double sr = getSampleRate();
    if (sr <= 0.0)
        return;

    const double m = (double) (patch->modFreq12 & 0x0fff);
    /** Maps 12-bit mod period to modulation LFO rate (audio-rate approximation). */
    const double modHz = 2.0 + (m / 4095.0) * 40.0;
    modPhaseInc = (modHz / sr) * (double) FDSPatch::waveSteps;
}

float FDSVoice::renderOneSample()
{
    if (patch == nullptr)
        return 0.f;

    const double sr = getSampleRate();
    if (sr <= 0.0)
        return 0.f;

    pitchWheelRatio = pitchWheelToRatio (currentPitchWheel);
    carrierHz = (double) juce::MidiMessage::getMidiNoteInHertz (currentMidiNote)
                * (double) pitchWheelRatio;

    volumeEnvGain = (float) patch->getVolumeGainClamped();
    modEnvGain = (float) patch->getModGainClamped();

    updateRatesFromPatch();

    modPhase += (float) modPhaseInc;
    while (modPhase >= (float) FDSPatch::waveSteps)
        modPhase -= (float) FDSPatch::waveSteps;

    const int modIndex = ((int) modPhase) % FDSPatch::waveSteps;
    applyModWaveStep (patch->modWave[(size_t) modIndex], modCounter);

    const float modNorm = (float) modCounter / 64.f;
    const float depth = (modEnvGain / 63.f) * 0.05f;
    const float carrierInc = (float) ((carrierHz / sr) * (double) FDSPatch::waveSteps)
                             * (1.f + depth * modNorm);

    carrierPhase += carrierInc;
    while (carrierPhase >= (float) FDSPatch::waveSteps)
        carrierPhase -= (float) FDSPatch::waveSteps;
    while (carrierPhase < 0.f)
        carrierPhase += (float) FDSPatch::waveSteps;

    const int waveIndex = ((int) carrierPhase) % FDSPatch::waveSteps;
    const float w = (float) patch->carrierWave[(size_t) waveIndex] / 63.f;
    const float bipolar = w * 2.f - 1.f;

    const float vol = (volumeEnvGain / 32.f) * velocity * masterGain;
    return bipolar * vol;
}

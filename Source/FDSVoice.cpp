/*
  ==============================================================================

    FDS-style wavetable voice (sample-rate aggregated DSP).

  ==============================================================================
*/

#include "FDSVoice.h"

#include "ModWavePreview.h"

#include <cmath>

//==============================================================================
void FDSVoice::ensureAdsrSampleRate()
{
    const double sr = getSampleRate();
    if (sr <= 0.0)
        return;

    if (sr == adsrSampleRate)
        return;

    carrierAdsr.setSampleRate (sr);
    modAdsr.setSampleRate (sr);

    if (patch != nullptr)
    {
        carrierAdsr.setParameters ({
            patch->carrierAttackSec,
            patch->carrierDecaySec,
            patch->carrierSustainLevel,
            patch->carrierReleaseSec
        });

        modAdsr.setParameters ({
            patch->modAttackSec,
            patch->modDecaySec,
            patch->modSustainLevel,
            patch->modReleaseSec
        });
    }

    adsrSampleRate = sr;
}

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
    currentMidiNote = midiNoteNumber;
    velocity = vel;
    currentPitchWheel = pitchWheelPosition;
    pitchWheelRatio = pitchWheelToRatio (currentPitchWheel);

    carrierPhase = 0.f;
    modPhase = 0.f;
    modCounter = 0;

    carrierHz = (double) juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber)
                * (double) pitchWheelRatio;

    const double sr = getSampleRate() > 0.0 ? getSampleRate() : 44100.0;
    carrierAdsr.setSampleRate (sr);
    modAdsr.setSampleRate (sr);
    adsrSampleRate = sr;

    if (patch != nullptr)
    {
        carrierAdsr.setParameters ({
            patch->carrierAttackSec,
            patch->carrierDecaySec,
            patch->carrierSustainLevel,
            patch->carrierReleaseSec
        });

        modAdsr.setParameters ({
            patch->modAttackSec,
            patch->modDecaySec,
            patch->modSustainLevel,
            patch->modReleaseSec
        });
    }
    else
    {
        carrierAdsr.setParameters ({});
        modAdsr.setParameters ({});
    }

    carrierAdsr.reset();
    modAdsr.reset();
    carrierAdsr.noteOn();
    modAdsr.noteOn();

    updateRatesFromPatch();
}

void FDSVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    if (! allowTailOff)
    {
        carrierAdsr.reset();
        modAdsr.reset();
        clearCurrentNote();
        return;
    }

    carrierAdsr.noteOff();
    modAdsr.noteOff();
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

    ensureAdsrSampleRate();

    const auto numChannels = outputBuffer.getNumChannels();

    while (--numSamples >= 0)
    {
        if (getCurrentlyPlayingNote() < 0)
            break;

        const float s = renderOneSample();

        for (int ch = 0; ch < numChannels; ++ch)
            outputBuffer.addSample (ch, startSample, s);

        ++startSample;
    }
}

//==============================================================================
float FDSVoice::pitchWheelToRatio (int wheel14) const
{
    const float semis = (patch != nullptr ? patch->pitchBendRangeSemis : 2.f);
    const float x = ((float) wheel14 - 8192.f) / 8192.f;
    return std::pow (2.f, x * semis / 12.f);
}

void FDSVoice::updateRatesFromPatch()
{
    if (patch == nullptr)
        return;

    const double sr = getSampleRate();
    if (sr <= 0.0)
        return;

    const double m = (double) (patch->modFreq12 & 0x0fff);
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

    updateRatesFromPatch();

    const float cEnv = carrierAdsr.getNextSample();
    const float mEnv = modAdsr.getNextSample();

    if (! carrierAdsr.isActive())
    {
        modAdsr.reset();
        clearCurrentNote();
        return 0.f;
    }

    modPhase += (float) modPhaseInc;
    while (modPhase >= (float) FDSPatch::waveSteps)
        modPhase -= (float) FDSPatch::waveSteps;

    const int modIndex = ((int) modPhase) % FDSPatch::waveSteps;
    MagicalFDS::applyModWaveOpcode (patch->modWave[(size_t) modIndex], modCounter);

    const float modNorm = (float) modCounter / 64.f;
    const float depth = mEnv * patch->modDepth * 0.05f;
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

    const float gain = patch->masterGainLinear;
    const float vol = cEnv * gain * velocity * masterGain;
    return bipolar * vol;
}

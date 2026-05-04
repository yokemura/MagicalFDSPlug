/*
  ==============================================================================

    FDS-style wavetable voice (sample-rate aggregated DSP).

  ==============================================================================
*/

#include "FDSVoice.h"

#include "FDSModulationUnit.h"
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
    modCounter = 0;
    modCpuCyclePool = 0.0;
    modAcc12 = 0;
    modWalkPhase = 0;

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

    const float cEnv = carrierAdsr.getNextSample();
    const float mEnv = modAdsr.getNextSample();

    if (! carrierAdsr.isActive())
    {
        modAdsr.reset();
        clearCurrentNote();
        return 0.f;
    }

    // ---- Mod unit: 16 CPU サイクル毎に mod 周波数を加算、12bit 越えでテーブル 1 ステップ ----
    modCpuCyclePool += MagicalFDS::fdsCpuClockNtsc / sr;
    const uint32_t pMod = (uint32_t) (patch->modFreq12 & 0x0fffu);

    while (modCpuCyclePool >= 16.0)
    {
        modCpuCyclePool -= 16.0;

        if (pMod == 0)
            continue;

        uint32_t sum = modAcc12 + pMod;
        if (sum >= 4096u)
        {
            sum -= 4096u;
            modWalkPhase = (modWalkPhase + 1u) & 63u;
            const int modIndex = (int) (modWalkPhase >> 1);
            MagicalFDS::applyModWaveOpcode (patch->modWave[(size_t) modIndex], modCounter);
        }
        modAcc12 = sum;
    }

    // ---- Carrier: MIDI 基準ピッチ -> 12bit、変調は Wiki の wave_pitch -> ティック Hz ----
    const uint32_t pitch12 = (uint32_t) MagicalFDS::carrierHzToPitch12 (carrierHz);
    const int modGain6 = (int) std::lround (
        (double) juce::jlimit (0.f, 1.f, mEnv * patch->modDepth) * 63.0);

    const uint32_t wavePitch20 = MagicalFDS::computeWavePitch20 (
        pitch12, modCounter, modGain6);

    const double fTick = MagicalFDS::wavePitchToWaveTickHz (wavePitch20);
    const float carrierInc = (float) (fTick / sr);

    carrierPhase += carrierInc;
    while (carrierPhase >= (float) FDSPatch::carrierWaveSteps)
        carrierPhase -= (float) FDSPatch::carrierWaveSteps;
    while (carrierPhase < 0.f)
        carrierPhase += (float) FDSPatch::carrierWaveSteps;

    const int waveIndex = ((int) carrierPhase) % FDSPatch::carrierWaveSteps;
    const float w = (float) patch->carrierWave[(size_t) waveIndex] / 63.f;
    const float bipolar = w * 2.f - 1.f;

    const float gain = patch->masterGainLinear;
    const float vol = cEnv * gain * velocity * masterGain;
    return bipolar * vol;
}

/*
  ==============================================================================

    PatchBuilder — APVTS → FDSPatch

  ==============================================================================
*/

#include "PatchBuilder.h"

#include "Parameters.h"

#include <array>
#include <cmath>

namespace MagicalFDS
{
namespace
{
    constexpr int carrierN = FDSPatch::carrierWaveSteps;

    // NSDL 由来の三角は +/-1 中心で累積が -4..+4 に留まり表示が平坦になるため、
    // 同じ立ち上がり/落ち形状を保ちつつ +4/-4（opcode 3/5）に置き換えてレンジを拡げる。
    static constexpr std::array<uint8_t, FDSPatch::modWaveSteps> kFdsModTriangle = {
        0, 5, 0, 5, 0, 5, 0, 5, 0, 3, 0, 3, 0, 3, 0, 3,
        0, 3, 0, 3, 0, 3, 0, 3, 0, 5, 0, 5, 0, 5, 0, 5
    };

    // +1 連打は 1 周で c が 0..32 程度に留まるため、+4（opcode 3）でランプを急にし表示・変調とも振れを大きくする。
    static constexpr std::array<uint8_t, FDSPatch::modWaveSteps> kFdsModSaw = {
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
    };

    //==============================================================================
    float getFloatParam (juce::AudioProcessorValueTreeState& apvts, const juce::String& id)
    {
        if (auto* p = dynamic_cast<juce::AudioParameterFloat*> (apvts.getParameter (id)))
            return p->get();

        jassertfalse;
        return 0.f;
    }

    int getIntParam (juce::AudioProcessorValueTreeState& apvts, const juce::String& id)
    {
        if (auto* p = dynamic_cast<juce::AudioParameterInt*> (apvts.getParameter (id)))
            return p->get();

        jassertfalse;
        return 0;
    }

    int getChoiceParam (juce::AudioProcessorValueTreeState& apvts, const juce::String& id)
    {
        if (auto* p = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (id)))
            return p->getIndex();

        jassertfalse;
        return 0;
    }

    int quantize6bitUnipolar (double u01)
    {
        const int v = (int) std::lround (juce::jlimit (0.0, 1.0, u01) * 63.0);
        return juce::jlimit (0, 63, v);
    }

    void writeCarrierFromBipolar (const float* bipolar, std::array<uint8_t, carrierN>& dest)
    {
        for (int i = 0; i < carrierN; ++i)
        {
            const float x = juce::jlimit (-1.f, 1.f, bipolar[(size_t) i]);
            const double u = ((double) x + 1.0) * 0.5;
            dest[(size_t) i] = (uint8_t) quantize6bitUnipolar (u);
        }
    }

    //--- Additive -----------------------------------------------------------
    void buildCarrierAdditive (juce::AudioProcessorValueTreeState& apvts,
                               std::array<uint8_t, carrierN>& dest)
    {
        float raw[carrierN]{};

        for (int i = 0; i < carrierN; ++i)
        {
            const double phase = juce::MathConstants<double>::twoPi * (double) i / (double) carrierN;
            double sum = 0.0;

            for (int h = 1; h <= ParamIDs::carrierDrawbarCount; ++h)
            {
                const float amp = getFloatParam (apvts, makeDrawbarId (h - 1));
                sum += (double) amp * std::sin (phase * (double) h);
            }

            raw[i] = (float) sum;
        }

        float peak = 0.f;
        for (int i = 0; i < carrierN; ++i)
            peak = juce::jmax (peak, std::abs (raw[i]));

        const float scale = (peak > 1.f) ? (1.f / peak) : 1.f;

        float bipolar[carrierN]{};
        for (int i = 0; i < carrierN; ++i)
            bipolar[i] = juce::jlimit (-1.f, 1.f, raw[i] * scale);

        writeCarrierFromBipolar (bipolar, dest);
    }

    //--- Preset + morph -----------------------------------------------------
    float morphSignedPow (float x, float r)
    {
        const float ax = std::abs (x);
        if (ax < 1.0e-9f)
            return 0.f;

        const float s = (x >= 0.f) ? 1.f : -1.f;
        return s * (float) std::pow ((double) ax, (double) r);
    }

    void buildCarrierPresetMorph (juce::AudioProcessorValueTreeState& apvts,
                                  std::array<uint8_t, carrierN>& dest)
    {
        const int preset = getChoiceParam (apvts, ParamIDs::carrierPreset);
        const float morphAmount = getFloatParam (apvts, ParamIDs::carrierMorphAmount);

        float bipolar[carrierN]{};

        for (int i = 0; i < carrierN; ++i)
        {
            const float ph = (float) i / (float) carrierN;
            float x = 0.f;

            switch (preset)
            {
                case ParamChoices::PresetTriangle:
                {
                    x = (ph < 0.5f) ? (4.f * ph - 1.f) : (3.f - 4.f * ph);
                    break;
                }
                case ParamChoices::PresetSawtooth:
                {
                    x = 2.f * ph - 1.f;
                    break;
                }
                case ParamChoices::PresetSine:
                default:
                {
                    x = std::sin (juce::MathConstants<float>::twoPi * ph);
                    break;
                }
            }

            if (std::abs (morphAmount) > 1.0e-6f)
            {
                const float r = juce::jlimit (-10.f, 10.f, morphAmount);
                x = morphSignedPow (x, r);
            }

            bipolar[i] = juce::jlimit (-1.f, 1.f, x);
        }

        writeCarrierFromBipolar (bipolar, dest);
    }

    //--- Pulse --------------------------------------------------------------
    void buildCarrierPulse (juce::AudioProcessorValueTreeState& apvts,
                            std::array<uint8_t, carrierN>& dest)
    {
        const int w = juce::jlimit (1, 63, getIntParam (apvts, ParamIDs::carrierPulseWidth));

        for (int i = 0; i < carrierN; ++i)
            dest[(size_t) i] = (i < w) ? 63 : 0;
    }

    //--- Free draw ----------------------------------------------------------
    void buildCarrierFreeDraw (juce::AudioProcessorValueTreeState& apvts,
                                 std::array<uint8_t, carrierN>& dest)
    {
        for (int i = 0; i < carrierN; ++i)
            dest[(size_t) i] = (uint8_t) juce::jlimit (0, 63, getIntParam (apvts, makeFreeDrawId (i)));
    }
} // namespace

//==============================================================================
void applyApvtsToPatch (juce::AudioProcessorValueTreeState& apvts, FDSPatch& patch)
{
    const int mode = getChoiceParam (apvts, ParamIDs::carrierMode);

    switch (mode)
    {
        case ParamChoices::ModeAdditive:
            buildCarrierAdditive (apvts, patch.carrierWave);
            break;
        case ParamChoices::ModePresetMorph:
            buildCarrierPresetMorph (apvts, patch.carrierWave);
            break;
        case ParamChoices::ModePulseShape:
            buildCarrierPulse (apvts, patch.carrierWave);
            break;
        case ParamChoices::ModeFreeDraw:
            buildCarrierFreeDraw (apvts, patch.carrierWave);
            break;
        default:
            buildCarrierAdditive (apvts, patch.carrierWave);
            break;
    }

    const int modWaveIdx = getChoiceParam (apvts, ParamIDs::modWaveType);
    if (modWaveIdx == ParamChoices::ModWaveTriangle)
        patch.modWave = kFdsModTriangle;
    else
        patch.modWave = kFdsModSaw;

    const int rateUse = getChoiceParam (apvts, ParamIDs::modRateUse);
    const int rateRaw = getIntParam (apvts, ParamIDs::modRate);

    if (rateUse == ParamChoices::ModRateOff)
        patch.modFreq12 = 0;
    else if (rateUse == ParamChoices::ModRateLFO)
        patch.modFreq12 = (uint16_t) juce::jlimit (1, 30, rateRaw);
    else
        patch.modFreq12 = (uint16_t) juce::jlimit (30, 4095, rateRaw);
}

//==============================================================================
void applyRuntimeParametersFromApvts (juce::AudioProcessorValueTreeState& apvts, FDSPatch& patch)
{
    patch.pitchBendRangeSemis = (float) juce::jlimit (0, 24, getIntParam (apvts, ParamIDs::bendRange));
    patch.masterGainLinear    = juce::jlimit (0.f, 1.f, getFloatParam (apvts, ParamIDs::gain));
    patch.modDepth            = juce::jlimit (0, 63, getIntParam (apvts, ParamIDs::modDepth));
    patch.modRateUseIndex     = getChoiceParam (apvts, ParamIDs::modRateUse);

    patch.carrierAttackSec    = getFloatParam (apvts, ParamIDs::carrierA);
    patch.carrierDecaySec     = getFloatParam (apvts, ParamIDs::carrierD);
    patch.carrierSustainLevel = juce::jlimit (0.f, 1.f, getFloatParam (apvts, ParamIDs::carrierS));
    patch.carrierReleaseSec   = getFloatParam (apvts, ParamIDs::carrierR);

    patch.modAttackSec        = getFloatParam (apvts, ParamIDs::modA);
    patch.modDecaySec         = getFloatParam (apvts, ParamIDs::modD);
    patch.modSustainLevel     = juce::jlimit (0.f, 1.f, getFloatParam (apvts, ParamIDs::modS));
    patch.modReleaseSec       = getFloatParam (apvts, ParamIDs::modR);

    if (auto* p = dynamic_cast<juce::AudioParameterBool*> (apvts.getParameter (ParamIDs::lowpassEnabled)))
        patch.lowpassEnabled = p->get();
}

} // namespace MagicalFDS

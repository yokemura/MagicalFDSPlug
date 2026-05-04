/*
  ==============================================================================

    Shared FDS patch data (wavetables + register mirrors). Referenced by all
    FDSSound / FDSVoice instances.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include <array>
#include <cmath>

//==============================================================================
/** Register-sized mirror of FDS state shared across voices (patch layer). */
struct FDSPatch
{
    /** メイン wavetable RAM 64×6bit（実機 $4040-$407F）。 */
    static constexpr int carrierWaveSteps = 64;
    /** モジュレータ用 32×3bit 命令列（実機の mod テーブル長）。 */
    static constexpr int modWaveSteps = 32;

    std::array<uint8_t, carrierWaveSteps> carrierWave{};
    std::array<uint8_t, modWaveSteps> modWave{};

    /** $4080 volume envelope / gain control (raw register byte). */
    uint8_t reg4080 = 0xff;

    /** $4084 modulator envelope (raw register byte). */
    uint8_t reg4084 = 0xff;

    /** $408A envelope master speed (raw register byte). BIOS default $E8. */
    uint8_t reg408A = 0xe8;

    /** $4089 wavetable write enable / output flags (subset used here). */
    uint8_t reg4089 = 0x00;

    /** $4086 low byte + $4087 high nybble => 12-bit modulator timer period. */
    uint16_t modFreq12 = 0x0100;

    // ---- Runtime (from APVTS, updated each block; not part of FDS hardware mirror) ----
    float pitchBendRangeSemis = 2.f;
    /** UI Gain 0..1 (M8BP2 準拠リニア). */
    float masterGainLinear = 0.5f;
    /** 変調深さ 0..1：モジュレータ ADSR 出力に掛けた後 0..63 の実機 mod gain に量子化して Wiki 変調式へ渡す。 */
    float modDepth = 0.f;

    /** マスター出力 ~2kHz 1-pole LPF（仕様デフォルト ON）。 */
    bool lowpassEnabled = true;

    float carrierAttackSec = 0.f;
    float carrierDecaySec = 0.f;
    float carrierSustainLevel = 1.f;
    float carrierReleaseSec = 0.f;

    float modAttackSec = 0.f;
    float modDecaySec = 0.f;
    float modSustainLevel = 1.f;
    float modReleaseSec = 0.f;

    void resetToDefaults()
    {
        reg4080 = 0xff;
        reg4084 = 0xff;
        reg408A = 0xe8;
        reg4089 = 0x00;
        modFreq12 = 0x0100;
        lowpassEnabled = true;

        for (int i = 0; i < carrierWaveSteps; ++i)
        {
            const double p = juce::MathConstants<double>::twoPi * (double) i / (double) carrierWaveSteps;
            const int v = (int) std::lround (31.5 + 31.5 * std::sin (p));
            carrierWave[(size_t) i] = (uint8_t) juce::jlimit (0, 63, v);
        }

        modWave.fill (0);
    }

    /** Effective carrier gain 0..32 for output (after hardware clamp). */
    int getVolumeGainClamped() const
    {
        return juce::jmin ((int) (reg4080 & 0x3f), 32);
    }

    /** Effective mod envelope gain 0..63. */
    int getModGainClamped() const
    {
        return juce::jmin ((int) (reg4084 & 0x3f), 63);
    }
};

//==============================================================================
/** Synthesiser sound that ties voices to a shared @ref FDSPatch. */
class FDSSound final : public juce::SynthesiserSound
{
public:
    explicit FDSSound (FDSPatch* patchIn) : patch (patchIn) {}

    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }

    FDSPatch* getPatch() const { return patch; }

private:
    FDSPatch* patch = nullptr;
};

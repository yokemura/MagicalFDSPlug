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
    static constexpr int waveSteps = 32;

    std::array<uint8_t, waveSteps> carrierWave{};
    std::array<uint8_t, waveSteps> modWave{};

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

    void resetToDefaults()
    {
        reg4080 = 0xff;
        reg4084 = 0xff;
        reg408A = 0xe8;
        reg4089 = 0x00;
        modFreq12 = 0x0100;

        for (int i = 0; i < waveSteps; ++i)
        {
            const double p = juce::MathConstants<double>::twoPi * (double) i / (double) waveSteps;
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

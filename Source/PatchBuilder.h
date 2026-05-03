/*
  ==============================================================================

    Maps AudioProcessorValueTreeState parameters into FDSPatch (carrier /
    modulator wavetables and mod frequency). See docs/UI-Specification.md.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "FDSPatch.h"

namespace MagicalFDS
{

/** Fill @a patch from current APVTS values (safe to call on the audio thread). */
void applyApvtsToPatch (juce::AudioProcessorValueTreeState& apvts, FDSPatch& patch);

/** Wavetable以外: ゲイン・ベンドレンジ・ADSR 秒・変調深さを @a patch に書き込む。 */
void applyRuntimeParametersFromApvts (juce::AudioProcessorValueTreeState& apvts, FDSPatch& patch);

} // namespace MagicalFDS

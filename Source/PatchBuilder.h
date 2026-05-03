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

/** Fill @a patch from current APVTS values (safe to call from the audio thread). */
void applyApvtsToPatch (juce::AudioProcessorValueTreeState& apvts, FDSPatch& patch);

} // namespace MagicalFDS

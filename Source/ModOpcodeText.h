/*
  ==============================================================================

    ModOpcodeText.h
    Parse / format FDS modulator 32-step opcode text (digits 0..7, whitespace ignored).

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "FDSPatch.h"
#include "Parameters.h"

#include <array>

namespace MagicalFDS
{

struct ModOpcodeParseResult
{
    std::array<uint8_t, FDSPatch::modWaveSteps> opcodes {};
    bool ok = false;
    juce::String error;
};

inline bool isFullWidthDigit (juce::juce_wchar c)
{
    return c >= 0xff10 && c <= 0xff19;
}

inline ModOpcodeParseResult parseModOpcodeText (const juce::String& text)
{
    ModOpcodeParseResult result;
    juce::String compact;

    for (int i = 0; i < text.length(); ++i)
    {
        const juce::juce_wchar c = text[i];

        if (juce::CharacterFunctions::isWhitespace (c))
            continue;

        if (isFullWidthDigit (c))
        {
            result.error = "Full-width digits are not allowed.";
            return result;
        }

        if (c >= L'0' && c <= L'7')
        {
            compact += (juce::juce_wchar) c;
            continue;
        }

        result.error = "Invalid character '" + juce::String::charToString ((juce::juce_wchar) c)
                       + "' at position " + juce::String (i + 1) + ".";
        return result;
    }

    if (compact.isEmpty())
    {
        result.error = "Expected 32 digits (0-7), got 0.";
        return result;
    }

    if (compact.length() != FDSPatch::modWaveSteps)
    {
        result.error = "Expected 32 digits (0-7), got " + juce::String (compact.length()) + ".";
        return result;
    }

    for (int i = 0; i < FDSPatch::modWaveSteps; ++i)
        result.opcodes[(size_t) i] = (uint8_t) (compact[i] - L'0');

    result.ok = true;
    return result;
}

inline juce::String formatModOpcodeText (const std::array<uint8_t, FDSPatch::modWaveSteps>& opcodes)
{
    juce::String s;
    s.preallocateBytes (FDSPatch::modWaveSteps);

    for (int i = 0; i < FDSPatch::modWaveSteps; ++i)
        s += juce::String ((int) opcodes[(size_t) i]);

    return s;
}

inline std::array<uint8_t, FDSPatch::modWaveSteps> readModOpcodesFromApvts (
    juce::AudioProcessorValueTreeState& apvts)
{
    std::array<uint8_t, FDSPatch::modWaveSteps> out {};

    for (int i = 0; i < FDSPatch::modWaveSteps; ++i)
    {
        if (auto* p = dynamic_cast<juce::AudioParameterInt*> (apvts.getParameter (makeModOpcodeId (i))))
            out[(size_t) i] = (uint8_t) juce::jlimit (0, 7, p->get());
    }

    return out;
}

inline void writeModOpcodesToApvts (juce::AudioProcessorValueTreeState& apvts,
                                    const std::array<uint8_t, FDSPatch::modWaveSteps>& opcodes)
{
    for (int i = 0; i < FDSPatch::modWaveSteps; ++i)
    {
        auto* p = dynamic_cast<juce::AudioParameterInt*> (apvts.getParameter (makeModOpcodeId (i)));
        if (p == nullptr)
            continue;

        const int v = (int) opcodes[(size_t) i];
        if (p->get() == v)
            continue;

        p->beginChangeGesture();
        p->setValueNotifyingHost (p->getNormalisableRange().convertTo0to1 ((float) v));
        p->endChangeGesture();
    }
}

inline bool isModCustomOpcodesActive (juce::AudioProcessorValueTreeState& apvts)
{
    if (auto* p = dynamic_cast<juce::AudioParameterBool*> (
            apvts.getParameter (ParamIDs::modCustomOpcodesActive)))
        return p->get();

    return false;
}

inline void setModCustomOpcodesActive (juce::AudioProcessorValueTreeState& apvts, bool active)
{
    auto* p = dynamic_cast<juce::AudioParameterBool*> (
        apvts.getParameter (ParamIDs::modCustomOpcodesActive));
    if (p == nullptr || p->get() == active)
        return;

    p->beginChangeGesture();
    p->setValueNotifyingHost (active ? 1.f : 0.f);
    p->endChangeGesture();
}

} // namespace MagicalFDS

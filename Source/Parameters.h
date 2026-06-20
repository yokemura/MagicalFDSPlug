/*
  ==============================================================================

    Parameters.h
    APVTS parameter IDs and layout factory for MagicalFDSPlug.

    Range/default values are derived from Magical8bitPlug2 where applicable
    (see docs/UI-Specification.md for the spec). Undetermined defaults are marked
    TODO in code where applicable.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include <array>
#include <memory>
#include <vector>

namespace MagicalFDS
{
inline constexpr int maxPolyphony = 32;

//==============================================================================
namespace ParamIDs
{
    // ---- Global ----
    inline constexpr const char* gain           = "gain";
    inline constexpr const char* polyphony      = "polyphony";
    inline constexpr const char* bendRange      = "bendRange";
    inline constexpr const char* colorTheme     = "colorTheme";
    inline constexpr const char* lowpassEnabled = "lowpassEnabled";

    // ---- Carrier ADSR ----
    inline constexpr const char* carrierA = "carrierA";
    inline constexpr const char* carrierD = "carrierD";
    inline constexpr const char* carrierS = "carrierS";
    inline constexpr const char* carrierR = "carrierR";

    // ---- Carrier waveform mode ----
    inline constexpr const char* carrierMode = "carrierMode";

    // ---- (1) Additive: 8 drawbars (8' .. 1') ----
    // IDs: carrierDrawbar0 .. carrierDrawbar7
    inline constexpr const char* carrierDrawbarPrefix = "carrierDrawbar";
    inline constexpr int         carrierDrawbarCount  = 8;

    // ---- (2) Preset morph ----
    inline constexpr const char* carrierPreset      = "carrierPreset";
    inline constexpr const char* carrierMorphAmount = "carrierMorphAmount";

    // ---- (3) Pulse ----
    inline constexpr const char* carrierPulseWidth = "carrierPulseWidth";

    // ---- (4) FreeDraw: 64 steps (FDS キャリア wavetable 長), 6bit (0..63) ----
    // IDs: carrierFreeDraw00 .. carrierFreeDraw63 (zero-padded to 2 digits)
    inline constexpr const char* carrierFreeDrawPrefix = "carrierFreeDraw";
    inline constexpr int         carrierFreeDrawCount  = 64;

    // ---- Modulator ADSR ----
    inline constexpr const char* modA = "modA";
    inline constexpr const char* modD = "modD";
    inline constexpr const char* modS = "modS";
    inline constexpr const char* modR = "modR";

    // ---- Modulator controls ----
    inline constexpr const char* modWaveType = "modWaveType";
    /** プリセット 0..5 のうち直近選択（Custom 未コミット時の opcode フォールバック）。 */
    inline constexpr const char* modLastPresetWaveType = "modLastPresetWaveType";
    /** Custom opcode 列が少なくとも 1 回有効コミット済み。 */
    inline constexpr const char* modCustomOpcodesActive = "modCustomOpcodesActive";
    /** Custom: 32×3bit opcode（modOpcode00 .. modOpcode31）。 */
    inline constexpr const char* modOpcodePrefix = "modOpcode";
    inline constexpr int         modOpcodeCount  = 32;
    /** LFO / FM / OFF — OFF のとき音源は modFreq12=0（変調ユニット停止）。 */
    inline constexpr const char* modRateUse = "modRateUse";
    inline constexpr const char* modRate     = "modRate";
    inline constexpr const char* modDepth    = "modDepth";
}

//==============================================================================
namespace ParamChoices
{
    inline juce::StringArray colorThemeNames()
    {
        return { "FDS", "YMCK", "YMCK Dark", "Japan", "Worldwide", "Monotone", "Mono Dark" };
    }

    inline juce::StringArray carrierModeNames()
    {
        return { "Additive", "Preset Morph", "Pulse Shape", "Free Draw" };
    }

    inline juce::StringArray carrierPresetNames()
    {
        return { "Triangle", "Sawtooth", "Sine" };
    }

    inline juce::StringArray modWaveNames()
    {
        return { "Triangle", "Sawtooth", "Sine", "Square", "Rise", "Fall", "Custom" };
    }

    /** modLastPresetWaveType 用（Custom を除く 6 プリセット）。 */
    inline juce::StringArray modPresetWaveNames()
    {
        return { "Triangle", "Sawtooth", "Sine", "Square", "Rise", "Fall" };
    }

    enum CarrierMode    { ModeAdditive = 0, ModePresetMorph, ModePulseShape, ModeFreeDraw };
    enum CarrierPreset  { PresetTriangle = 0, PresetSawtooth, PresetSine };
    enum ModWaveType    { ModWaveTriangle = 0, ModWaveSawtooth,
                          ModWaveSine, ModWaveSquare,
                          ModWaveOneShotUp, ModWaveOneShotDown,
                          ModWaveCustom };

    inline juce::StringArray modRateUseNames()
    {
        return { "LFO", "FM", "OFF" };
    }

    /** modRateUse の列順と一致（既定は従来どおり FM + レート 256 に近い挙動）。 */
    enum ModRateUse { ModRateLFO = 0, ModRateFM, ModRateOff };
}

//==============================================================================
/** Build the parameter ID for the i-th additive drawbar (i=0..7). */
inline juce::String makeDrawbarId (int index)
{
    return juce::String (ParamIDs::carrierDrawbarPrefix) + juce::String (index);
}

/** Build the parameter ID for the i-th free-draw step (i=0..63, two-digit zero-padded). */
inline juce::String makeFreeDrawId (int index)
{
    return juce::String (ParamIDs::carrierFreeDrawPrefix) + juce::String::formatted ("%02d", index);
}

/** Build the parameter ID for the i-th modulator opcode step (i=0..31, two-digit zero-padded). */
inline juce::String makeModOpcodeId (int index)
{
    return juce::String (ParamIDs::modOpcodePrefix) + juce::String::formatted ("%02d", index);
}

//==============================================================================
/** Create the APVTS layout for the plugin's parameters. */
inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    using APF = juce::AudioParameterFloat;
    using API = juce::AudioParameterInt;
    using APB = juce::AudioParameterBool;
    using APC = juce::AudioParameterChoice;

    constexpr int v = 1; // ParameterID version hint (M8BP2 convention)

    // Shared range for all ADSR A/D/R sliders (Magical8bitPlug2 准拠).
    const juce::NormalisableRange<float> adrRange { 0.0f, 5.0f, 0.001f, 0.5f };
    // Gain / Sustain / drawbars: 0..1 リニア、表示は ADR と同様 3 桁（step 0.001）。
    const juce::NormalisableRange<float> levelRange { 0.0f, 1.0f, 0.001f };

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    //--- Global ---------------------------------------------------------------
    params.push_back (std::make_unique<APF> (
        juce::ParameterID { ParamIDs::gain, v },
        "Gain",
        levelRange, 0.5f));

    params.push_back (std::make_unique<APF> (
        juce::ParameterID { ParamIDs::polyphony, v },
        "Polyphony",
        juce::NormalisableRange<float> (1.0f, (float) maxPolyphony, 1.0f, 1.0f), 1.0f));

    params.push_back (std::make_unique<API> (
        juce::ParameterID { ParamIDs::bendRange, v },
        "Bend Range", 0, 24, 2));

    params.push_back (std::make_unique<APC> (
        juce::ParameterID { ParamIDs::colorTheme, v },
        "Color Theme", ParamChoices::colorThemeNames(), 0));

    params.push_back (std::make_unique<APB> (
        juce::ParameterID { ParamIDs::lowpassEnabled, v },
        "Lowpass", true));

    //--- Carrier ADSR ---------------------------------------------------------
    params.push_back (std::make_unique<APF> (
        juce::ParameterID { ParamIDs::carrierA, v }, "Carrier Attack",  adrRange, 0.0f));
    params.push_back (std::make_unique<APF> (
        juce::ParameterID { ParamIDs::carrierD, v }, "Carrier Decay",   adrRange, 0.0f));
    params.push_back (std::make_unique<APF> (
        juce::ParameterID { ParamIDs::carrierS, v }, "Carrier Sustain",
        levelRange, 1.0f));
    params.push_back (std::make_unique<APF> (
        juce::ParameterID { ParamIDs::carrierR, v }, "Carrier Release", adrRange, 0.0f));

    //--- Carrier waveform mode ------------------------------------------------
    params.push_back (std::make_unique<APC> (
        juce::ParameterID { ParamIDs::carrierMode, v },
        "Carrier Wave Mode", ParamChoices::carrierModeNames(), ParamChoices::ModeAdditive));

    //--- (1) Additive: 8 drawbars --------------------------------------------
    // 倍音 8', 4', 2 2/3', 2', 1 3/5', 1 1/3', 1 1/7', 1' に相当。
    static const std::array<const char*, ParamIDs::carrierDrawbarCount> drawbarLabels {
        "8'", "4'", "2 2/3'", "2'", "1 3/5'", "1 1/3'", "1 1/7'", "1'"
    };
    for (int i = 0; i < ParamIDs::carrierDrawbarCount; ++i)
    {
        // 8' のみ 1.0、他倍音は 0（加算合成の素直な初期値）。
        const float defaultLevel = (i == 0 ? 1.0f : 0.0f);
        params.push_back (std::make_unique<APF> (
            juce::ParameterID { makeDrawbarId (i), v },
            juce::String ("Drawbar ") + drawbarLabels[(size_t) i],
            levelRange, defaultLevel));
    }

    //--- (2) Preset morph -----------------------------------------------------
    params.push_back (std::make_unique<APC> (
        juce::ParameterID { ParamIDs::carrierPreset, v },
        "Carrier Preset", ParamChoices::carrierPresetNames(), ParamChoices::PresetTriangle));

    params.push_back (std::make_unique<APF> (
        juce::ParameterID { ParamIDs::carrierMorphAmount, v },
        "Morph Amount",
        juce::NormalisableRange<float> (-1.0f, 1.0f, 0.001f), 0.0f));

    //--- (3) Pulse ------------------------------------------------------------
    // 64 点テーブル中央付近（32）を既定。
    params.push_back (std::make_unique<API> (
        juce::ParameterID { ParamIDs::carrierPulseWidth, v },
        "Pulse Width", 1, 63, 32));

    //--- (4) FreeDraw: 64 steps (wavetable 1 sample per step) ----------------
    // 既定は中点 32。方式切替時に音源側の現在値で上書きする想定のため、初回値は任意でよい。
    for (int i = 0; i < ParamIDs::carrierFreeDrawCount; ++i)
    {
        params.push_back (std::make_unique<API> (
            juce::ParameterID { makeFreeDrawId (i), v },
            juce::String ("Free Draw ") + juce::String (i),
            0, 63, 32));
    }

    //--- Modulator ADSR -------------------------------------------------------
    params.push_back (std::make_unique<APF> (
        juce::ParameterID { ParamIDs::modA, v }, "Mod Attack",  adrRange, 0.0f));
    params.push_back (std::make_unique<APF> (
        juce::ParameterID { ParamIDs::modD, v }, "Mod Decay",   adrRange, 0.0f));
    params.push_back (std::make_unique<APF> (
        juce::ParameterID { ParamIDs::modS, v }, "Mod Sustain",
        levelRange, 1.0f));
    params.push_back (std::make_unique<APF> (
        juce::ParameterID { ParamIDs::modR, v }, "Mod Release", adrRange, 0.0f));

    //--- Modulator controls ---------------------------------------------------
    params.push_back (std::make_unique<APC> (
        juce::ParameterID { ParamIDs::modWaveType, v },
        "Mod Wave Type", ParamChoices::modWaveNames(), ParamChoices::ModWaveTriangle));

    params.push_back (std::make_unique<APC> (
        juce::ParameterID { ParamIDs::modLastPresetWaveType, v },
        "Mod Last Preset", ParamChoices::modPresetWaveNames(), ParamChoices::ModWaveTriangle));

    params.push_back (std::make_unique<APB> (
        juce::ParameterID { ParamIDs::modCustomOpcodesActive, v },
        "Mod Custom Opcodes Active", false));

    // Custom opcode 列の APVTS 既定は三角波プリセット相当（kFdsModTriangle と一致）。
    static constexpr std::array<uint8_t, ParamIDs::modOpcodeCount> kDefaultModOpcodes {
        0, 5, 0, 5, 0, 5, 0, 5, 0, 3, 0, 3, 0, 3, 0, 3,
        0, 3, 0, 3, 0, 3, 0, 3, 0, 5, 0, 5, 0, 5, 0, 5
    };
    for (int i = 0; i < ParamIDs::modOpcodeCount; ++i)
    {
        params.push_back (std::make_unique<API> (
            juce::ParameterID { makeModOpcodeId (i), v },
            juce::String ("Mod Opcode ") + juce::String (i),
            0, 7, (int) kDefaultModOpcodes[(size_t) i]));
    }

    params.push_back (std::make_unique<APC> (
        juce::ParameterID { ParamIDs::modRateUse, v },
        "Mod Rate Use", ParamChoices::modRateUseNames(), ParamChoices::ModRateFM));

    // LFO 時 UI は 1..30、FM 時は 30..4095。内部は常に 1..4095（モード切替でクランプ）。
    params.push_back (std::make_unique<API> (
        juce::ParameterID { ParamIDs::modRate, v },
        "Mod Rate", 1, 4095, 256));

    // 実機 mod gain 相当 0..63（UI 値を DSP にそのまま渡す）。
    params.push_back (std::make_unique<API> (
        juce::ParameterID { ParamIDs::modDepth, v },
        "Mod Depth", 0, 63, 0));

    return { params.begin(), params.end() };
}

} // namespace MagicalFDS

/*
  ==============================================================================

    ModWavePreview.h
    FDS モジュレータ 3bit 命令列（実機どおり 32 ステップ）の累積波形プレビュー用レベル（0..63）。
    音源側 applyModWaveOpcode と同じオペコード意味・ラップ規則。

  ==============================================================================
*/

#pragma once

#include <array>
#include <cmath>

#include "FDSPatch.h"

namespace MagicalFDS
{

/** 1 ステップ分の 3bit 波形命令を累積カウンタに適用（FDSVoice と同一）。 */
inline void applyModWaveOpcode (uint8_t code, int& counter)
{
    switch (code & 7u)
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

/**
 * テーブル 1 周分を先頭から適用したあとの累積値を各ステップで 0..63 に写像。
 * @a out[i] は @a modWave[0..i] を順に適用した直後のカウンタに対応。
 */
inline void buildModWavePreviewLevels63 (const std::array<uint8_t, FDSPatch::modWaveSteps>& modWave,
                                          std::array<uint8_t, FDSPatch::modWaveSteps>& out)
{
    int c = 0;

    for (int i = 0; i < FDSPatch::modWaveSteps; ++i)
    {
        applyModWaveOpcode (modWave[(size_t) i], c);
        const int u = (int) std::lround (((double) c + 64.0) * 63.0 / 127.0);
        out[(size_t) i] = (uint8_t) juce::jlimit (0, 63, u);
    }
}

} // namespace MagicalFDS

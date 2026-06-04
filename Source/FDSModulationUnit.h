/*
  ==============================================================================

    FDSModulationUnit.h
    NESdev Wiki FDS_audio の変調ユニット・波形ティック周波数の式（NTSC CPU クロック準拠）。
    https://www.nesdev.org/wiki/FDS_audio

  ==============================================================================
*/

#pragma once

#include <cmath>
#include <cstdint>

namespace MagicalFDS
{

/** NTSC 約 1.789773 MHz（Wiki の記載値）。 */
inline constexpr double fdsCpuClockNtsc = 1789773.0;

/**
    MIDI 由来のキャリア Hz と、wavetable 64 サンプルを 1 可聴周期とみなす場合の
    無変調波形ティック周波数 f_tick = carrierHz * 64 に対応する 12bit pitch p。
    f_tick = n * p / 16 / 4096 より逆算。実機レジスタ照合用（ホットパスでは未使用）。
*/
inline int carrierHzToPitch12 (double carrierHz, double cpuClock = fdsCpuClockNtsc)
{
    const double fTick = carrierHz * 64.0;
    const double p = fTick * 16.0 * 4096.0 / cpuClock;
    const int pi = (int) std::lround (p);

    if (pi < 0)
        return 0;
    if (pi > 4095)
        return 4095;

    return pi;
}

/**
    Wiki「Modulation unit」の変調係数 temp（8bit、無変調時 64）。
    counter: $4085 相当 7bit 符号付き、gain: 6bit mod gain。
*/
inline uint32_t computeModulationTemp8 (int modCounter7, int modGain6)
{
    int gain = modGain6;
    if (gain < 0) gain = 0;
    if (gain > 63) gain = 63;

    int ctr = modCounter7;
    if (ctr < -64) ctr = -64;
    if (ctr > 63) ctr = 63;

    int32_t temp = ctr * gain;

    if (((temp & 0x0f) != 0) && ((temp & 0x800) == 0))
        temp += 0x20;

    temp += 0x400;
    return (uint32_t) ((temp >> 4) & 0xff);
}

/**
    キャリア波形ティック周波数 Hz。12bit pitch 飽和を使わずフル MIDI を追従。
    飽和なしのとき f_tick = carrierHz * temp（無変調 temp=64 => carrierHz*64）。
*/
inline double carrierHzToTickHz (double carrierHz, int modCounter7, int modGain6)
{
    const uint32_t temp = computeModulationTemp8 (modCounter7, modGain6);
    return carrierHz * (double) temp;
}

/**
    Wiki「Modulation unit」の C 風コードに準じた 20bit wave_pitch。
    pitch: $4082/4083 相当 12bit、counter: $4085 相当 7bit 符号付き、gain: 6bit mod gain。
*/
inline uint32_t computeWavePitch20 (uint32_t pitch12, int modCounter7, int modGain6)
{
    pitch12 &= 0xfffu;
    const uint32_t temp = computeModulationTemp8 (modCounter7, modGain6);
    return (uint32_t) ((pitch12 * temp) & 0xfffffu);
}

/** Wiki: f = n * w / 16 / 2^18 （wave table ティック周波数 Hz）。 */
inline double wavePitchToWaveTickHz (uint32_t wavePitch20, double cpuClock = fdsCpuClockNtsc)
{
    return cpuClock * (double) wavePitch20 / 16.0 / 262144.0;
}

} // namespace MagicalFDS

/*
  ==============================================================================

    LayoutConstants.h
    Layout / spacing constants inspired by Magical8bitPlug2.

  ==============================================================================
*/

#pragma once

namespace MagicalFDS::Layout
{
    // Window
    constexpr int totalWidth   = 800;
    constexpr int outerMargin  = 10; // left/right/top
    constexpr int bottomMargin = 20;

    // Separators / gaps
    constexpr int sectionSeparatorHeight = 16;
    constexpr int verticalSeparatorWidth = 16;
    constexpr int componentMargin        = 2;

    // Typography / rows (match Magical8bitPlug2 rhythm)
    constexpr int sectionHeaderHeight = 22;
    constexpr int rowHeight           = 28;

    // Label column
    constexpr int labelColumnWidth = 60;
    constexpr int labelControlGap  = 8;

    // Column widths (3:2 ratio inside inner area)
    constexpr int innerWidth   = totalWidth - outerMargin * 2; // 780
    constexpr int carrierWidth = (innerWidth - verticalSeparatorWidth) * 3 / 5;
    constexpr int modWidth     = innerWidth - verticalSeparatorWidth - carrierWidth;

    // Wave display (3:2 aspect)
    constexpr int carrierWaveDisplayWidth  = 360;
    constexpr int carrierWaveDisplayHeight = 240;

    constexpr int modWaveDisplayWidth  = 252;
    constexpr int modWaveDisplayHeight = 168;
}


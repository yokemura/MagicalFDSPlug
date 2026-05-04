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
    /** Space between the section title row and the controls below. */
    constexpr int sectionHeadingContentGap = 8;
    constexpr int rowHeight           = 28;

    // Label column
    constexpr int labelColumnWidth = 60;
    constexpr int labelControlGap  = 8;

    /** Gap between Global section left and right columns (at the right edge of the left block). */
    constexpr int globalSectionColumnMargin = 40;

    // Column widths (3:2 ratio inside inner area)
    constexpr int innerWidth   = totalWidth - outerMargin * 2; // 780
    constexpr int carrierWidth = (innerWidth - verticalSeparatorWidth) * 3 / 5;
    constexpr int modWidth     = innerWidth - verticalSeparatorWidth - carrierWidth;

    // Wave display (3:2 aspect)
    constexpr int carrierWaveDisplayWidth  = 360;
    constexpr int carrierWaveDisplayHeight = 240;

    constexpr int modWaveDisplayWidth  = 252;
    constexpr int modWaveDisplayHeight = 168;

    /** キャリア波形コントロールパートの高さ（仕様: 波形パートと同じ）。 */
    constexpr int carrierControlPanelHeight = carrierWaveDisplayHeight;

    constexpr int adsrBlockHeight = 4 * rowHeight;

    /** キャリア列の縦積み（見出し+ギャップ+ADSR+余白+モード+余白+波形+余白+コントロール）。 */
    constexpr int carrierColumnContentHeight = sectionHeaderHeight + sectionHeadingContentGap + adsrBlockHeight + componentMargin + rowHeight
                                               + componentMargin + carrierWaveDisplayHeight + componentMargin
                                               + carrierControlPanelHeight;

    /** 上余白 + グローバル + 区切り + キャリア列 + 下余白。 */
    constexpr int editorHeightHint = outerMargin + sectionHeaderHeight + sectionHeadingContentGap + rowHeight * 3 + sectionSeparatorHeight
                                     + carrierColumnContentHeight + bottomMargin + rowHeight + componentMargin;
}


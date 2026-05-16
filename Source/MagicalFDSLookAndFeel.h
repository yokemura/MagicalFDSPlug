/*
  ==============================================================================

    MagicalFDSLookAndFeel.h
    Magical8bitPlug2 風の配色とカスタム描画（スライダー／コンボ／トグル／見出し）。

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ColorScheme.h"

namespace MagicalFDS
{
class MagicalFDSLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    void applyColorScheme (const ColorScheme& newScheme);

    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle style,
                           juce::Slider& slider) override;

    void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox& box) override;

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;

    void drawTickBox (juce::Graphics& g, juce::Component& component,
                      float x, float y, float w, float h,
                      bool ticked,
                      bool isEnabled,
                      bool shouldDrawButtonAsHighlighted,
                      bool shouldDrawButtonAsDown) override;

    void drawLabel (juce::Graphics& g, juce::Label& label) override;

    juce::Font getComboBoxFont (juce::ComboBox& box) override;

    juce::Label* createSliderTextBox (juce::Slider& slider) override;

private:
    ColorScheme scheme { kColorSchemeFds };
};
} // namespace MagicalFDS

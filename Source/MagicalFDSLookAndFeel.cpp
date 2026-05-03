/*
  ==============================================================================

    MagicalFDSLookAndFeel.cpp

  ==============================================================================
*/

#include "MagicalFDSLookAndFeel.h"

#include "ui/SectionLabel.h"

namespace MagicalFDS
{
namespace
{
    constexpr float kSliderTrackThickness = 6.f;
    constexpr float kSliderCorner         = 3.f;
    constexpr float kComboCorner          = 4.f;
}

//==============================================================================
void MagicalFDSLookAndFeel::applyColorScheme (const ColorScheme& newScheme)
{
    scheme = newScheme;

    using Id = juce::ComboBox::ColourIds;

    setColour (juce::ResizableWindow::backgroundColourId, scheme.background);
    setColour (juce::Label::textColourId, scheme.main);
    setColour (juce::Label::outlineColourId, juce::Colours::transparentBlack);
    setColour (juce::TextEditor::textColourId, scheme.main);
    setColour (juce::TextEditor::backgroundColourId, scheme.textBoxFill);
    setColour (juce::TextEditor::outlineColourId, scheme.genericBorder);
    setColour (juce::Slider::thumbColourId, scheme.accent);
    setColour (juce::Slider::trackColourId, scheme.mainThinLine);
    setColour (juce::Slider::backgroundColourId, scheme.boxFill);
    setColour (juce::Slider::textBoxTextColourId, scheme.main);
    setColour (juce::Slider::textBoxBackgroundColourId, scheme.textBoxFill);
    setColour (juce::Slider::textBoxOutlineColourId, scheme.genericBorder);
    setColour (Id::backgroundColourId, scheme.textBoxFill);
    setColour (Id::outlineColourId, scheme.genericBorder);
    setColour (Id::textColourId, scheme.main);
    setColour (Id::arrowColourId, scheme.main);
    setColour (juce::ToggleButton::textColourId, scheme.main);
    setColour (juce::ToggleButton::tickColourId, scheme.accent);
    setColour (juce::ToggleButton::tickDisabledColourId, scheme.mainDarkened);
    setColour (juce::TextButton::buttonColourId, scheme.boxFill);
    setColour (juce::TextButton::textColourOffId, scheme.main);
    setColour (juce::TextButton::textColourOnId, scheme.reversedForeground);
    setColour (juce::ComboBox::backgroundColourId, scheme.textBoxFill);
    setColour (juce::PopupMenu::backgroundColourId, scheme.textBoxFill);
    setColour (juce::PopupMenu::textColourId, scheme.main);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, scheme.main);
    setColour (juce::PopupMenu::highlightedTextColourId, scheme.reversedForeground);
}

//==============================================================================
juce::Label* MagicalFDSLookAndFeel::createSliderTextBox (juce::Slider& slider)
{
    auto* l = LookAndFeel_V4::createSliderTextBox (slider);

    // 左列ラベルと同じテキスト色（LookAndFeel の Label::textColourId）。V4 のグレースキーム特例で白固定になるのを避ける。
    const auto textCol = slider.findColour (juce::Label::textColourId, true);
    l->setColour (juce::Label::textColourId, textCol);
    l->setColour (juce::TextEditor::textColourId, textCol);
    l->setColour (juce::Label::outlineColourId, juce::Colours::transparentBlack);

    return l;
}

//==============================================================================
void MagicalFDSLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos,
                                              float minSliderPos,
                                              float maxSliderPos,
                                              const juce::Slider::SliderStyle style,
                                              juce::Slider& slider)
{
    if (slider.isBar())
    {
        LookAndFeel_V4::drawLinearSlider (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        return;
    }

    if (style != juce::Slider::LinearHorizontal && style != juce::Slider::LinearVertical)
    {
        LookAndFeel_V4::drawLinearSlider (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        return;
    }

    const auto isTwoVal   = (style == juce::Slider::TwoValueVertical   || style == juce::Slider::TwoValueHorizontal);
    const auto isThreeVal = (style == juce::Slider::ThreeValueVertical || style == juce::Slider::ThreeValueHorizontal);

    if (isTwoVal || isThreeVal)
    {
        LookAndFeel_V4::drawLinearSlider (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        return;
    }

    const auto bgCol    = slider.findColour (juce::Slider::backgroundColourId);
    const auto fillCol  = slider.findColour (juce::Slider::trackColourId);
    const auto thumbCol = slider.findColour (juce::Slider::thumbColourId);
    const auto lineCol  = slider.findColour (juce::Slider::textBoxOutlineColourId);

    if (slider.isHorizontal())
    {
        const float cy = (float) y + (float) height * 0.5f;
        const float padX = 5.f;
        auto track = juce::Rectangle<float> ((float) x + padX,
                                              cy - kSliderTrackThickness * 0.5f,
                                              (float) width - 2.f * padX,
                                              kSliderTrackThickness);

        g.setColour (bgCol);
        g.fillRoundedRectangle (track, kSliderCorner);

        const float sx = juce::jlimit (track.getX(), track.getRight(), sliderPos);
        auto filled = track.withLeft (track.getX()).withRight (sx);
        if (filled.getWidth() > 0.5f)
        {
            g.setColour (fillCol);
            g.fillRoundedRectangle (filled, kSliderCorner);
        }

        const float thumbD = juce::jmin (12.f, (float) height - 4.f, (float) width * 0.12f);
        auto thumb = juce::Rectangle<float> (thumbD, thumbD).withCentre ({ sx, cy });

        g.setColour (thumbCol);
        g.fillEllipse (thumb);
        g.setColour (lineCol.withAlpha (0.45f));
        g.drawEllipse (thumb, 1.0f);
    }
    else
    {
        const float cx = (float) x + (float) width * 0.5f;
        const float padY = 5.f;
        auto track = juce::Rectangle<float> (cx - kSliderTrackThickness * 0.5f,
                                              (float) y + padY,
                                              kSliderTrackThickness,
                                              (float) height - 2.f * padY);

        g.setColour (bgCol);
        g.fillRoundedRectangle (track, kSliderCorner);

        const float sy = juce::jlimit (track.getY(), track.getBottom(), sliderPos);
        auto filled = juce::Rectangle<float> (track.getX(), sy, track.getWidth(), track.getBottom() - sy);
        if (filled.getHeight() > 0.5f)
        {
            g.setColour (fillCol);
            g.fillRoundedRectangle (filled, kSliderCorner);
        }

        const float thumbD = juce::jmin (12.f, (float) width - 4.f, (float) height * 0.12f);
        auto thumb = juce::Rectangle<float> (thumbD, thumbD).withCentre ({ cx, sy });

        g.setColour (thumbCol);
        g.fillEllipse (thumb);
        g.setColour (lineCol.withAlpha (0.45f));
        g.drawEllipse (thumb, 1.0f);
    }
}

//==============================================================================
void MagicalFDSLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                                          int buttonX, int buttonY, int buttonW, int buttonH,
                                          juce::ComboBox& box)
{
    juce::ignoreUnused (isButtonDown);

    const bool inPropertySheet = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr;
    const float corner = inPropertySheet ? 0.f : kComboCorner;

    auto bounds = juce::Rectangle<float> (0.f, 0.f, (float) width, (float) height).reduced (0.5f, 0.5f);

    g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle (bounds, corner);

    g.setColour (juce::Colours::white.withAlpha (box.isEnabled() ? 0.14f : 0.06f));
    g.fillRoundedRectangle (bounds.removeFromTop (juce::jmin (2.f, bounds.getHeight() * 0.22f)), corner * 0.5f);

    g.setColour (box.findColour (juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle (juce::Rectangle<float> (0.5f, 0.5f, (float) width - 1.f, (float) height - 1.f), corner, 1.2f);

    juce::Rectangle<float> arrowZone;

    if (buttonW > 0 && buttonH > 0)
        arrowZone = juce::Rectangle<float> ((float) buttonX, (float) buttonY, (float) buttonW, (float) buttonH);
    else
        arrowZone = juce::Rectangle<float> ((float) width - 28.f, 0.f, 22.f, (float) height);

    juce::Path path;
    path.startNewSubPath (arrowZone.getX() + 4.f, arrowZone.getCentreY() - 2.f);
    path.lineTo (arrowZone.getCentreX(), arrowZone.getCentreY() + 3.f);
    path.lineTo (arrowZone.getRight() - 4.f, arrowZone.getCentreY() - 2.f);

    g.setColour (box.findColour (juce::ComboBox::arrowColourId).withAlpha (box.isEnabled() ? 0.95f : 0.25f));
    g.strokePath (path, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

juce::Font MagicalFDSLookAndFeel::getComboBoxFont (juce::ComboBox& box)
{
    return withDefaultMetrics (juce::FontOptions { juce::jmin (14.5f, (float) box.getHeight() * 0.78f) });
}

//==============================================================================
void MagicalFDSLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                              bool shouldDrawButtonAsHighlighted,
                                              bool shouldDrawButtonAsDown)
{
    if (button.getRadioGroupId() != 0)
    {
        auto bounds = button.getLocalBounds().toFloat();
        const float side = juce::jmin (bounds.getHeight() - 10.f, 17.f);
        auto circle = juce::Rectangle<float> (side, side).withCentre ({ bounds.getX() + 6.f + side * 0.5f,
                                                                        bounds.getCentreY() });

        if (shouldDrawButtonAsDown)
            g.setColour (scheme.main.withAlpha (0.08f));
        else if (shouldDrawButtonAsHighlighted)
            g.setColour (scheme.main.withAlpha (0.05f));

        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            g.fillEllipse (circle.expanded (1.f));

        g.setColour (button.findColour (juce::ToggleButton::tickDisabledColourId).withAlpha (0.35f));
        g.fillEllipse (circle);
        g.setColour (scheme.genericBorder);
        g.drawEllipse (circle, 1.2f);

        if (button.getToggleState())
        {
            g.setColour (button.findColour (juce::ToggleButton::tickColourId));
            const float inset = juce::jmax (2.5f, side * 0.28f);
            g.fillEllipse (circle.reduced (inset));
        }

        g.setColour (button.findColour (juce::ToggleButton::textColourId));
        g.setFont (withDefaultMetrics (juce::FontOptions { juce::jmin (15.f, bounds.getHeight() * 0.72f) }));

        if (! button.isEnabled())
            g.setOpacity (0.5f);

        const int textLeft = juce::roundToInt (circle.getRight() + 8.f);
        g.drawFittedText (button.getButtonText(),
                          textLeft, 0, juce::jmax (1, button.getWidth() - textLeft - 2), button.getHeight(),
                          juce::Justification::centredLeft,
                          10);
        return;
    }

    LookAndFeel_V4::drawToggleButton (g, button, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
}

void MagicalFDSLookAndFeel::drawTickBox (juce::Graphics& g, juce::Component& component,
                                         float x, float y, float w, float h,
                                         const bool ticked,
                                         const bool isEnabled,
                                         const bool shouldDrawButtonAsHighlighted,
                                         const bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused (isEnabled, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

    const juce::Rectangle<float> tickBounds (x, y, w, h);

    g.setColour (component.findColour (juce::ToggleButton::tickDisabledColourId).withAlpha (0.22f));
    g.fillRoundedRectangle (tickBounds, 3.5f);

    g.setColour (component.findColour (juce::ToggleButton::tickDisabledColourId));
    g.drawRoundedRectangle (tickBounds.reduced (0.5f), 3.5f, 1.0f);

    if (ticked)
    {
        g.setColour (component.findColour (juce::ToggleButton::tickColourId));
        auto tick = getTickShape (0.78f);
        g.fillPath (tick, tick.getTransformToScaleToFit (tickBounds.reduced (5.f, 6.f).toFloat(), false));
    }
}

//==============================================================================
void MagicalFDSLookAndFeel::drawLabel (juce::Graphics& g, juce::Label& label)
{
    if (dynamic_cast<MagicalFDS::UI::SectionLabel*> (&label) != nullptr)
    {
        auto b = label.getLocalBounds().toFloat();
        const float accentW = 4.f;
        auto main = b;
        const auto accentBar = main.removeFromLeft (accentW);

        g.setColour (scheme.accent);
        g.fillRoundedRectangle (accentBar, 1.2f);

        g.setColour (scheme.mainThinLine.withAlpha (0.55f));
        g.drawLine (main.getX(), main.getBottom() - 1.f, main.getRight(), main.getBottom() - 1.f, 1.f);

        g.setColour (label.findColour (juce::Label::textColourId, true));
        g.setFont (label.getFont().boldened());
        g.drawText (label.getText(), main.reduced (6.f, 0).toNearestInt(), label.getJustificationType(), true);
        return;
    }

    LookAndFeel_V4::drawLabel (g, label);
}
} // namespace MagicalFDS

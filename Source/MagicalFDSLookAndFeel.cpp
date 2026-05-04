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

    using CB = juce::ComboBox::ColourIds;

    setColour (juce::ResizableWindow::backgroundColourId, scheme.background);

    // Magical8bitPlug2::applyLookAndFeel と同じ割当（Label / TextEditor / Toggle / TextButton / Combo）
    setColour (juce::Label::textColourId, scheme.mainThinLine);
    setColour (juce::Label::outlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Label::textWhenEditingColourId, scheme.mainThinLine);

    setColour (juce::TextEditor::textColourId, scheme.mainThinLine);
    setColour (juce::TextEditor::backgroundColourId, scheme.textBoxFill);
    setColour (juce::TextEditor::outlineColourId, scheme.genericBorder);

    setColour (juce::Slider::thumbColourId, scheme.accent);
    setColour (juce::Slider::trackColourId, scheme.main);
    setColour (juce::Slider::backgroundColourId, scheme.mainDarkened);
    setColour (juce::Slider::textBoxTextColourId, scheme.mainThinLine);
    setColour (juce::Slider::textBoxBackgroundColourId, scheme.textBoxFill);
    setColour (juce::Slider::textBoxHighlightColourId, scheme.mainThinLine);
    setColour (juce::Slider::textBoxOutlineColourId, scheme.genericBorder);

    setColour (juce::TextButton::buttonColourId, scheme.boxFill);
    setColour (juce::TextButton::textColourOffId, scheme.reversedForeground);
    setColour (juce::TextButton::textColourOnId, scheme.reversedForeground);

    setColour (juce::ToggleButton::textColourId, scheme.mainThinLine);
    setColour (juce::ToggleButton::tickColourId, scheme.mainThinLine);
    setColour (juce::ToggleButton::tickDisabledColourId, scheme.mainThinLine);

    setColour (CB::backgroundColourId, scheme.boxFill);
    setColour (CB::outlineColourId, scheme.genericBorder);
    setColour (CB::textColourId, scheme.reversedForeground);
    setColour (CB::arrowColourId, scheme.reversedForeground);
    setColour (CB::buttonColourId, scheme.mainThinLine);

    // 8bit 未指定: コンボのポップアップは読みやすさのため textBox 系で維持
    setColour (juce::PopupMenu::backgroundColourId, scheme.textBoxFill);
    setColour (juce::PopupMenu::textColourId, scheme.mainThinLine);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, scheme.main);
    setColour (juce::PopupMenu::highlightedTextColourId, scheme.reversedForeground);
}

//==============================================================================
juce::Label* MagicalFDSLookAndFeel::createSliderTextBox (juce::Slider& slider)
{
    auto* l = LookAndFeel_V4::createSliderTextBox (slider);

    const auto textCol    = slider.findColour (juce::Slider::textBoxTextColourId, true);
    const auto fillCol   = slider.findColour (juce::Slider::textBoxBackgroundColourId, true);
    const auto borderCol = slider.findColour (juce::Slider::textBoxOutlineColourId, true);

    l->setColour (juce::Label::textColourId, textCol);
    l->setColour (juce::Label::backgroundColourId, fillCol);
    l->setColour (juce::Label::outlineColourId, borderCol);
    l->setColour (juce::TextEditor::textColourId, textCol);
    l->setColour (juce::TextEditor::backgroundColourId, fillCol);
    l->setColour (juce::TextEditor::outlineColourId, borderCol);

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
        const auto& props = slider.getProperties();
        const bool bipolarMorph = props.contains ("magicalFdsBipolarMorph")
                                  && (bool) props["magicalFdsBipolarMorph"];

        if (bipolarMorph)
        {
            const double minV = slider.getMinimum();
            const double maxV = slider.getMaximum();
            const double span = maxV - minV;
            const float t0 = (float) ((span > 1.0e-12) ? (0.0 - minV) / span : 0.5);
            const float centreX = juce::jlimit (track.getX(), track.getRight(), track.getX() + t0 * track.getWidth());
            const float lo = juce::jmin (centreX, sx);
            const float hi = juce::jmax (centreX, sx);
            auto filled = juce::Rectangle<float> (lo, track.getY(), hi - lo, track.getHeight());

            if (filled.getWidth() > 0.5f)
            {
                g.setColour (fillCol);
                g.fillRoundedRectangle (filled, kSliderCorner);
            }
        }
        else
        {
            auto filled = track.withLeft (track.getX()).withRight (sx);
            if (filled.getWidth() > 0.5f)
            {
                g.setColour (fillCol);
                g.fillRoundedRectangle (filled, kSliderCorner);
            }
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
    juce::ignoreUnused (isButtonDown, buttonX, buttonY, buttonW, buttonH);

    const bool inPropertySheet = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr;
    const float corner = inPropertySheet ? 0.f : kComboCorner;

    auto bounds = juce::Rectangle<float> (0.f, 0.f, (float) width, (float) height).reduced (0.5f, 0.5f);

    g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle (bounds, corner);

    g.setColour (juce::Colours::white.withAlpha (box.isEnabled() ? 0.14f : 0.06f));
    g.fillRoundedRectangle (bounds.removeFromTop (juce::jmin (2.f, bounds.getHeight() * 0.22f)), corner * 0.5f);

    g.setColour (box.findColour (juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle (juce::Rectangle<float> (0.5f, 0.5f, (float) width - 1.f, (float) height - 1.f), corner, 1.2f);

    // JUCE の ComboBox::paint は (label->getRight() 〜 右端) を button 矩形として渡すため、
    // その矩形を矢印ゾーンにすると三角が横いっぱいに伸びる。LookAndFeel_V4 と同様、常に右 30×20 の領域だけ使う。
    const auto arrowZone = juce::Rectangle<float> ((float) width - 30.f, 0.f, 20.f, (float) height);

    juce::Path path;
    path.startNewSubPath (arrowZone.getX() + 3.f, arrowZone.getCentreY() - 2.f);
    path.lineTo (arrowZone.getCentreX(), arrowZone.getCentreY() + 3.f);
    path.lineTo (arrowZone.getRight() - 3.f, arrowZone.getCentreY() - 2.f);

    g.setColour (box.findColour (juce::ComboBox::arrowColourId).withAlpha (box.isEnabled() ? 0.9f : 0.2f));
    g.strokePath (path, juce::PathStrokeType (2.0f));
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

    // Slider の値ラベルは V4 の drawLabel がグレースキーム由来の配色で上書きすることがあるため、
    // Slider::textBox* の色で自前描画する（Magical8bitPlug2 の TextBoxRight 相当）。
    if (auto* slider = dynamic_cast<juce::Slider*> (label.getParentComponent()))
    {
        if (label.getCurrentTextEditor() == nullptr)
        {
            constexpr float kValueBoxCorner = 3.f;
            auto bounds = label.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

            g.setColour (slider->findColour (juce::Slider::textBoxBackgroundColourId, true));
            g.fillRoundedRectangle (bounds, kValueBoxCorner);

            g.setColour (slider->findColour (juce::Slider::textBoxOutlineColourId, true));
            g.drawRoundedRectangle (bounds, kValueBoxCorner, 1.f);

            g.setColour (slider->findColour (juce::Slider::textBoxTextColourId, true)
                             .withMultipliedAlpha (label.isEnabled() ? 1.f : 0.45f));
            g.setFont (label.getFont());
            g.drawFittedText (label.getText(),
                              label.getLocalBounds().reduced (2, 0),
                              label.getJustificationType(),
                              juce::jmax (1, label.getText().length()));
            return;
        }
    }

    LookAndFeel_V4::drawLabel (g, label);
}
} // namespace MagicalFDS

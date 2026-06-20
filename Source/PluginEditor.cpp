/*
  ==============================================================================

    PluginEditor.cpp

  ==============================================================================
*/

#include "PluginEditor.h"

#include "ColorScheme.h"
#include "LayoutConstants.h"
#include "Parameters.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p)
    , audioProcessor (p)
    , globalSection (p.getParameters())
    , carrierSection (p.getParameters())
    , modulatorSection (p.getParameters())
{
    setLookAndFeel (&lookAndFeel);

    addAndMakeVisible (globalSection);
    addAndMakeVisible (carrierSection);
    addAndMakeVisible (modulatorSection);

    modulatorSection.setOnPreferredHeightChanged ([this] { updateEditorSize(); });

    applyThemeFromParameters();

    audioProcessor.getParameters().addParameterListener (MagicalFDS::ParamIDs::colorTheme, this);

    updateEditorSize();
}

void NewProjectAudioProcessorEditor::updateEditorSize()
{
    setSize (MagicalFDS::Layout::totalWidth,
             MagicalFDS::Layout::editorHeightForModColumn (modulatorSection.getPreferredHeight()));
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
    audioProcessor.getParameters().removeParameterListener (MagicalFDS::ParamIDs::colorTheme, this);
    setLookAndFeel (nullptr);
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (findColour (juce::ResizableWindow::backgroundColourId));
}

void NewProjectAudioProcessorEditor::resized()
{
    using namespace MagicalFDS::Layout;

    auto content = getLocalBounds();
    content.removeFromTop (outerMargin);
    content.removeFromLeft (outerMargin);
    content.removeFromRight (outerMargin);
    content.removeFromBottom (bottomMargin);

    constexpr int globalBlockHeight = sectionHeaderHeight + sectionHeadingContentGap + rowHeight * 3;

    globalSection.setBounds (content.removeFromTop (globalBlockHeight));
    content.removeFromTop (sectionSeparatorHeight);

    const int columnH = content.getHeight();
    auto carrierArea = content.removeFromLeft (carrierWidth);
    content.removeFromLeft (verticalSeparatorWidth);

    carrierSection.setBounds (carrierArea.withHeight (columnH));
    modulatorSection.setBounds (content.withHeight (columnH));
}

void NewProjectAudioProcessorEditor::parameterChanged (const juce::String& parameterID, float newValue)
{
    juce::ignoreUnused (newValue);

    if (parameterID == MagicalFDS::ParamIDs::colorTheme)
        applyThemeFromParameters();
}

void NewProjectAudioProcessorEditor::applyThemeFromParameters()
{
    int idx = 0;

    if (auto* p = dynamic_cast<juce::AudioParameterChoice*> (
            audioProcessor.getParameters().getParameter (MagicalFDS::ParamIDs::colorTheme)))
        idx = p->getIndex();

    static constexpr std::array<MagicalFDS::ColorSchemeType, 7> map {
        MagicalFDS::kColorSchemeFds,
        MagicalFDS::kColorSchemeYmck,
        MagicalFDS::kColorSchemeYmckDark,
        MagicalFDS::kColorSchemeFamicom,
        MagicalFDS::kColorSchemeNes,
        MagicalFDS::kColorSchemeMonotoneLight,
        MagicalFDS::kColorSchemeMonotoneDark
    };

    idx = juce::jlimit (0, (int) map.size() - 1, idx);
    const MagicalFDS::ColorScheme scheme (map[(size_t) idx]);
    lookAndFeel.applyColorScheme (scheme);
    repaint();
}

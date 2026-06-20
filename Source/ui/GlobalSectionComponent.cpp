/*
  ==============================================================================

    GlobalSectionComponent.cpp

  ==============================================================================
*/

#include "GlobalSectionComponent.h"

#include "../LayoutConstants.h"
#include "../Parameters.h"
#include "../PluginProcessor.h"

namespace MagicalFDS::UI
{
namespace
{
    constexpr const char* kPresetFileExtension = "*.xml";
    constexpr const char* kDefaultPresetFileName = "MagicalFDSPlug.xml";
}

GlobalSectionComponent::GlobalSectionComponent (NewProjectAudioProcessor& processor,
                                                std::function<void()> onStateLoaded)
    : audioProcessor (processor)
    , onStateLoadedCallback (std::move (onStateLoaded))
    , gain (processor.getParameters(), ParamIDs::gain, "Gain")
    , polyphony (processor.getParameters(), ParamIDs::polyphony, "Polyphony")
    , bendRange (processor.getParameters(), ParamIDs::bendRange, "Bend")
    , colorTheme (processor.getParameters(), ParamIDs::colorTheme, "Theme")
    , lowpass (processor.getParameters(), ParamIDs::lowpassEnabled, "LPF")
{
    addAndMakeVisible (heading);
    addAndMakeVisible (gain);
    addAndMakeVisible (polyphony);
    addAndMakeVisible (bendRange);
    addAndMakeVisible (loadButton);
    addAndMakeVisible (saveButton);
    addAndMakeVisible (colorTheme);
    addAndMakeVisible (lowpass);

    loadButton.onClick = [this] { showLoadDialog(); };
    saveButton.onClick = [this] { showSaveDialog(); };
}

void GlobalSectionComponent::resized()
{
    auto r = getLocalBounds();

    heading.setBounds (r.removeFromTop (Layout::sectionHeaderHeight));
    r.removeFromTop (Layout::sectionHeadingContentGap);

    const int leftW = (r.getWidth() * 3) / 4;
    auto leftCol = r.removeFromLeft (leftW);
    r.removeFromLeft (Layout::globalSectionColumnMargin);

    gain.setBounds (leftCol.removeFromTop (Layout::rowHeight));
    polyphony.setBounds (leftCol.removeFromTop (Layout::rowHeight));
    bendRange.setBounds (leftCol.removeFromTop (Layout::rowHeight));

    auto buttonRow = r.removeFromTop (Layout::rowHeight);
    const int halfW = buttonRow.getWidth() / 2;
    loadButton.setBounds (buttonRow.removeFromLeft (halfW).reduced (Layout::componentMargin));
    saveButton.setBounds (buttonRow.reduced (Layout::componentMargin));

    colorTheme.setBounds (r.removeFromTop (Layout::rowHeight));
    lowpass.setBounds (r.removeFromTop (Layout::rowHeight));
}

void GlobalSectionComponent::showSaveDialog()
{
    fileChooser = std::make_unique<juce::FileChooser> (
        "Save preset",
        juce::File::getSpecialLocation (juce::File::userDocumentsDirectory).getChildFile (kDefaultPresetFileName),
        kPresetFileExtension);

    const auto flags = juce::FileBrowserComponent::saveMode
                     | juce::FileBrowserComponent::canSelectFiles
                     | juce::FileBrowserComponent::warnAboutOverwriting;

    fileChooser->launchAsync (flags, [this] (const juce::FileChooser& chooser)
    {
        const auto file = chooser.getResult();
        if (file == juce::File())
            return;

        if (! audioProcessor.saveStateToXmlFile (file))
        {
            juce::AlertWindow::showMessageBoxAsync (
                juce::AlertWindow::WarningIcon,
                "Save failed",
                "Could not write the preset file.");
        }
    });
}

void GlobalSectionComponent::showLoadDialog()
{
    fileChooser = std::make_unique<juce::FileChooser> (
        "Load preset",
        juce::File::getSpecialLocation (juce::File::userDocumentsDirectory),
        kPresetFileExtension);

    const auto flags = juce::FileBrowserComponent::openMode
                     | juce::FileBrowserComponent::canSelectFiles;

    fileChooser->launchAsync (flags, [this] (const juce::FileChooser& chooser)
    {
        const auto file = chooser.getResult();
        if (file == juce::File())
            return;

        if (audioProcessor.loadStateFromXmlFile (file))
        {
            if (onStateLoadedCallback)
                onStateLoadedCallback();
        }
        else
        {
            juce::AlertWindow::showMessageBoxAsync (
                juce::AlertWindow::WarningIcon,
                "Load failed",
                "Could not read the preset file.");
        }
    });
}
} // namespace MagicalFDS::UI

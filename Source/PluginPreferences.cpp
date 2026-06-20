/*
  ==============================================================================

    PluginPreferences.cpp

  ==============================================================================
*/

#include "PluginPreferences.h"

#include "Parameters.h"

namespace MagicalFDS::PluginPreferences
{
namespace
{
    juce::PropertiesFile::Options makeSettingsOptions()
    {
        juce::PropertiesFile::Options opts;
        opts.applicationName          = JucePlugin_Name;
        opts.filenameSuffix           = "settings";
        opts.folderName               = JucePlugin_Manufacturer;
        opts.osxLibrarySubFolder      = "Application Support";
        opts.commonToAllUsers         = false;
        opts.storageFormat            = juce::PropertiesFile::storeAsXML;
        opts.millisecondsBeforeSaving = -1;
        return opts;
    }

    juce::PropertiesFile openUserSettings()
    {
        const auto opts = makeSettingsOptions();
        return juce::PropertiesFile (opts.getDefaultFile(), opts);
    }
}

std::optional<int> getStoredColorThemeIndex()
{
    juce::PropertiesFile settings = openUserSettings();

    if (settings.containsKey (kColorThemeKey))
    {
        const int maxIndex = ParamChoices::colorThemeNames().size() - 1;
        return juce::jlimit (0, maxIndex, settings.getIntValue (kColorThemeKey, 0));
    }

    return std::nullopt;
}

void setStoredColorThemeIndex (int index)
{
    juce::PropertiesFile settings = openUserSettings();
    const int maxIndex = ParamChoices::colorThemeNames().size() - 1;
    settings.setValue (kColorThemeKey, juce::jlimit (0, maxIndex, index));
    settings.saveIfNeeded();
}

juce::File getLastPresetDirectory()
{
    juce::PropertiesFile settings = openUserSettings();
    const auto path = settings.getValue (kLastPresetDirectoryKey);

    if (path.isNotEmpty())
    {
        const juce::File dir (path);
        if (dir.isDirectory())
            return dir;
    }

    return {};
}

void setLastPresetDirectory (const juce::File& directory)
{
    if (! directory.isDirectory())
        return;

    juce::PropertiesFile settings = openUserSettings();
    settings.setValue (kLastPresetDirectoryKey, directory.getFullPathName());
    settings.saveIfNeeded();
}
} // namespace MagicalFDS::PluginPreferences

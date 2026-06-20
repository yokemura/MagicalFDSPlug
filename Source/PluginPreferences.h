/*
  ==============================================================================

    PluginPreferences.h
    User settings persisted to a shared .settings file (PropertiesFile XML).

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include <optional>

namespace MagicalFDS::PluginPreferences
{
inline constexpr const char* kColorThemeKey           = "colorThemeIndex";
inline constexpr const char* kLastPresetDirectoryKey  = "lastPresetDirectory";

std::optional<int> getStoredColorThemeIndex();
void               setStoredColorThemeIndex (int index);

juce::File getLastPresetDirectory();
void       setLastPresetDirectory (const juce::File& directory);
} // namespace MagicalFDS::PluginPreferences

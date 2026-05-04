/*
  ==============================================================================

    ColorScheme.cpp

  ==============================================================================
*/

#include "ColorScheme.h"

namespace MagicalFDS
{
ColorScheme::ColorScheme (ColorSchemeType type)
{
    switch (type)
    {
        case kColorSchemeYmck:
            background          = juce::Colour (0xffffffff);
            textBoxFill         = juce::Colour (0xfff4f4ff);
            boxFill             = juce::Colour (0xff00aeff);
            main                = juce::Colour (0xff00aeff);
            mainThinLine        = juce::Colour (0xff0086ff);
            mainDarkened        = juce::Colour (0xff0080bb);
            accent              = juce::Colour (0xffffdd00);
            reversedForeground  = juce::Colour (0xffffffff);
            genericBorder       = juce::Colour (0xff0080bb);
            warning             = juce::Colour (0xffe22be0);
            break;

        case kColorSchemeYmckDark:
            background          = juce::Colour (0xff000000);
            textBoxFill         = juce::Colour (0xff101010);
            boxFill             = juce::Colour (0xff004070);
            main                = juce::Colour (0xff00aeff);
            mainThinLine        = juce::Colour (0xff40A0ff);
            mainDarkened        = juce::Colour (0xff004070);
            accent              = juce::Colour (0xffffdd00);
            reversedForeground  = juce::Colour (0xffffffff);
            genericBorder       = juce::Colour (0xff0080bb);
            warning             = juce::Colour (0xffe22be0);
            break;

        case kColorSchemeFamicom:
            background          = juce::Colour (0xfff4f5ec);
            boxFill             = juce::Colour (0xffead780);
            textBoxFill         = juce::Colour (0xfffdfee9);
            main                = juce::Colour (0xffc04136);
            mainThinLine        = juce::Colour (0xff97372b);
            mainDarkened        = juce::Colour (0xffc5b0ae);
            accent              = juce::Colour (0xffeed27d);
            reversedForeground  = juce::Colour (0xff101010);
            genericBorder       = juce::Colour (0xff641c11);
            warning             = juce::Colour (0xff641c11);
            break;

        case kColorSchemeNes:
            background          = juce::Colour (0xffefefef);
            boxFill             = juce::Colour (0xff69616e);
            textBoxFill         = juce::Colour (0xffd1c8c3);
            main                = juce::Colour (0xff1b1718);
            mainThinLine        = juce::Colour (0xff000000);
            mainDarkened        = juce::Colour (0xffa1988f);
            accent              = juce::Colour (0xffc04136);
            reversedForeground  = juce::Colour (0xffffffff);
            genericBorder       = juce::Colour (0xffa1988f);
            warning             = juce::Colour (0xffc04136);
            break;

        case kColorSchemeMonotoneLight:
            background          = juce::Colour (0xffffffff);
            boxFill             = juce::Colour (0xffffffff);
            textBoxFill         = juce::Colour (0xfff0f0f0);
            main                = juce::Colour (0xff202020);
            mainThinLine        = juce::Colour (0xff000000);
            mainDarkened        = juce::Colour (0xffc0c0c0);
            accent              = juce::Colour (0xff808080);
            reversedForeground  = juce::Colour (0xff000000);
            genericBorder       = juce::Colour (0xff606060);
            warning             = juce::Colour (0xff000000);
            break;

        case kColorSchemeMonotoneDark:
            background          = juce::Colour (0xff000000);
            boxFill             = juce::Colour (0xff101010);
            textBoxFill         = juce::Colour (0xff000000);
            main                = juce::Colour (0xffffffff);
            mainThinLine        = juce::Colour (0xffffffff);
            mainDarkened        = juce::Colour (0xff404040);
            accent              = juce::Colour (0xffa0a0a0);
            reversedForeground  = juce::Colour (0xffffffff);
            genericBorder       = juce::Colour (0xffc0c0c0);
            warning             = juce::Colour (0xffffffff);
            break;

        case kColorSchemeFds:
            background          = juce::Colour (0xff1d1d1d);
            boxFill             = juce::Colour (0xffc04136);
            textBoxFill         = juce::Colour (0xff3c3c3c);
            main                = juce::Colour (0xffeed27d);
            mainThinLine        = juce::Colour (0xffeed27d);
            mainDarkened        = juce::Colour (0xff3c3c3c);
            accent              = juce::Colour (0xffeed27d);
            reversedForeground  = juce::Colour (0xff101010);
            genericBorder       = juce::Colour (0xff641c11);
            warning             = juce::Colour (0xff641c11);
            break;

        default:
            break;
    }
}
} // namespace MagicalFDS


#pragma once
#include "shared_plugin_helpers/shared_plugin_helpers.h"

class EmbeddedFonts
{
private:
    juce::Font montserratBlack900;
    juce::Font montserratBold700;
    juce::Font montserratRegular400;
    juce::Font montserratLight300;
    juce::Font cutiveRegular;

public:
    EmbeddedFonts();
    const juce::Font& getMontserratBlack900() const;
    const juce::Font& getMontserratBold700() const;
    const juce::Font& getMontserratRegular400() const;
    const juce::Font& getMontserratLight300() const;
    const juce::Font& getCutiveRegular() const;
};
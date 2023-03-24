#pragma once
#include "PluginProcessor.h"
#include "shared_plugin_helpers/shared_plugin_helpers.h"
#include "EmbeddedFonts.h"

class MatronHeaderUI : public juce::Component
{
public:
    explicit MatronHeaderUI(GrandMatronAudioProcessor&);
    void resized() override;
    void paint(juce::Graphics&) override;
    float getTitleFontSize(){ return 32.0f; };

private:

    EmbeddedFonts mEmbeddedFonts;
    std::unique_ptr<juce::Drawable> mDecoration;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MatronHeaderUI)
};
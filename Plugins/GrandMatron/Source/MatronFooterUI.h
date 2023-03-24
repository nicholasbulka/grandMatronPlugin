#pragma once
#include "PluginProcessor.h"
#include "shared_plugin_helpers/shared_plugin_helpers.h"
#include "EmbeddedFonts.h"

class MatronFooterUI : public juce::Component
{
public:
    explicit MatronFooterUI(GrandMatronAudioProcessor&);
    void resized() override;
    void paint(juce::Graphics&) override;
    float getFooterFontSize(){ return 10.0f; };

private:

    EmbeddedFonts mEmbeddedFonts;
    std::unique_ptr<juce::Drawable> mFooterDecorationL;
    std::unique_ptr<juce::Drawable> mFooterDecorationR;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MatronFooterUI)
};
#pragma once

#include <shared_plugin_helpers/shared_plugin_helpers.h>
#include "PluginProcessor.h"
#include "EmbeddedFonts.h"
#include "MatronLookAndFeel.h"
#include "MatronHeaderUI.h"
#include "MatronFooterUI.h"

class RotarySliderWithLabels : public juce::Slider
{
public:
    RotarySliderWithLabels(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) :
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                     juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(&rap),
        suffix(unitSuffix)
    {
        setLookAndFeel(&lnf);
    }

    ~RotarySliderWithLabels()
    {
        setLookAndFeel(nullptr);
    }

    struct LabelPos
    {
        float pos;
        juce::String label;
    };

    juce::Array<LabelPos> labels;

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    float getTextHeight() const { return 16.0f; }
    juce::String getDisplayString() const;

private:
    MatronLookAndFeel lnf;
    juce::RangedAudioParameter* param;
    juce::String suffix;
};

/**
*/
class GrandMatronAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    GrandMatronAudioProcessorEditor (GrandMatronAudioProcessor&);
    ~GrandMatronAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    std::vector<juce::Component*> getComps();

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GrandMatronAudioProcessor& mAudioProcessor;
    RotarySliderWithLabels mLowPassFreqSlider, mLowPassSlopeSlider;
    juce::AudioProcessorValueTreeState::SliderAttachment mLowPassFreqSliderAttachment, mLowPassSlopeSliderAttachment;

    EmbeddedFonts mEmbeddedFonts;
    std::unique_ptr<juce::Drawable> mTicks;
    std::unique_ptr<juce::Drawable> mMatronGearDot;
    MatronLookAndFeel matronLAF; // [1]

    MatronHeaderUI mMatronHeaderUI;
    MatronFooterUI mMatronFooterUI;

    float headerFooterHeight = 140.f;
    float cutoffRotaryKnobHeight = 200.f;
    float slopeChoiceRotaryKnobHeight = 200.f;

    float editorWidth = 480.f;
    float editorHeight = 2 * headerFooterHeight + cutoffRotaryKnobHeight + slopeChoiceRotaryKnobHeight;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrandMatronAudioProcessorEditor)
};
























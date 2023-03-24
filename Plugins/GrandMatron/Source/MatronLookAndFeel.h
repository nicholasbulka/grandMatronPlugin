#include "shared_plugin_helpers/shared_plugin_helpers.h"
#include "BinaryData.h"
class MatronLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MatronLookAndFeel();
    ~MatronLookAndFeel() override;
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;

private:
    std::unique_ptr<juce::Drawable> mTicks;
    std::unique_ptr<juce::Drawable> mMatronGearDot;
};
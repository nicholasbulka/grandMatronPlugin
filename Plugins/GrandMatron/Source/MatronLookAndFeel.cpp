#include "MatronLookAndFeel.h"

MatronLookAndFeel::MatronLookAndFeel()
{
    mTicks = juce::Drawable::createFromImageData (BinaryData::ticks_svg, BinaryData::ticks_svgSize);
    mMatronGearDot = juce::Drawable::createFromImageData (BinaryData::matronGearDot_svg, BinaryData::matronGearDot_svgSize);
}

MatronLookAndFeel::~MatronLookAndFeel()
{
}

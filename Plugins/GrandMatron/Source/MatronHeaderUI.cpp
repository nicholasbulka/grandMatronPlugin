#include "MatronHeaderUI.h"

MatronHeaderUI::MatronHeaderUI(GrandMatronAudioProcessor& p)
{
    mDecoration = juce::Drawable::createFromImageData (BinaryData::decoration_svg, BinaryData::decoration_svgSize);

}

void MatronHeaderUI::paint (juce::Graphics& g)
{
    auto center = getLocalBounds().getCentre();

    g.setFont(juce::Font (mEmbeddedFonts.getMontserratBold700()));
    g.setFont(getTitleFontSize());

    g.drawFittedText("GRAND MATRON",
                     getLocalBounds(), juce::Justification::centred, 1);
    auto decorationWidth = mDecoration->getWidth();
    auto decorationOffset = 30;

    mDecoration->drawAt(g,
                        center.getX() - (.5f * decorationWidth),
                        center.getY() + decorationOffset, 1.0f);

}

void MatronHeaderUI::resized()
{

}
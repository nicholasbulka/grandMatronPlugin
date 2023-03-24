#include "MatronFooterUI.h"

MatronFooterUI::MatronFooterUI(GrandMatronAudioProcessor& p)
{
    mFooterDecorationL = juce::Drawable::createFromImageData (BinaryData::footerDecorationL_svg, BinaryData::footerDecorationL_svgSize);
    mFooterDecorationR = juce::Drawable::createFromImageData (BinaryData::footerDecorationR_svg, BinaryData::footerDecorationR_svgSize);

}

void MatronFooterUI::paint (juce::Graphics& g)
{
    auto bottomLeft = getLocalBounds().getBottomLeft();
    auto bottomRight = getLocalBounds().getBottomRight();

    g.setFont(juce::Font (mEmbeddedFonts.getMontserratBold700()));
    g.setFont(getFooterFontSize());

    g.drawFittedText("dadsquad",
                     getLocalBounds(), juce::Justification::centredBottom, 1);
    auto decorationRWidth = mFooterDecorationR->getWidth();
    auto heightOffsetL = mFooterDecorationL->getHeight();
    auto heightOffsetR = mFooterDecorationR->getHeight();

    auto decorationOffset = 30;

    mFooterDecorationL->drawAt(g,
                        bottomLeft.getX(),
                        bottomLeft.getY() - heightOffsetL, 1.0f);

    mFooterDecorationR->drawAt(g,
                               bottomRight.getX() - decorationRWidth,
                               bottomRight.getY() - heightOffsetR, 1.0f);

}

void MatronFooterUI::resized()
{

}
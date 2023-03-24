#include "EmbeddedFonts.h"
#include "BinaryData.h"

EmbeddedFonts::EmbeddedFonts()
{
    montserratBlack900 = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::MontserratBlack900_ttf,
                                                                            BinaryData::MontserratBlack900_ttfSize));
    montserratBold700 = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::MontserratBold700_ttf,
                                                                           BinaryData::MontserratBold700_ttfSize));
    montserratRegular400 = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::MontserratRegular400_ttf,
                                                                              BinaryData::MontserratRegular400_ttfSize));
    montserratLight300 = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::MontserratLight300_ttf,
                                                                            BinaryData::MontserratLight300_ttfSize));
    cutiveRegular = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::CutiveRegular_ttf,
                                                                       BinaryData::CutiveRegular_ttfSize));
}

const juce::Font& EmbeddedFonts::getMontserratBlack900() const
{
    return montserratBlack900;
}
const juce::Font& EmbeddedFonts::getMontserratBold700() const
{
    return montserratBold700;
}
const juce::Font& EmbeddedFonts::getMontserratRegular400() const
{
    return montserratRegular400;
}
const juce::Font& EmbeddedFonts::getMontserratLight300() const
{
    return montserratLight300;
}

const juce::Font& EmbeddedFonts::getCutiveRegular() const
{
    return cutiveRegular;
}

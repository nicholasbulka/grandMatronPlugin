
#include "PluginProcessor.h"
#include "PluginEditor.h"

void MatronLookAndFeel::drawRotarySlider(juce::Graphics & g,
                                   int x,
                                   int y,
                                   int width,
                                   int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   juce::Slider & slider)
{

    auto bounds = juce::Rectangle<float>(x, y, width, height);

    if( auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider))
    {
        auto center = bounds.getCentre();
        auto gearWidth = mMatronGearDot->getWidth();
        auto gearHeight = mMatronGearDot->getHeight();
        auto tickWidth = mTicks->getWidth();
        auto tickHeight = mTicks->getHeight();
        //float tickWidthOffset = 20.f;
        float tickHeightOffset = 30.0f;
        float tickWidthOffset = 30.0f;

        auto drawableCenter = mMatronGearDot->getLocalBounds().getCentre();

        mTicks->drawAt(g, center.getX() - (tickWidth / 2) - tickWidthOffset, center.getY() - (tickHeight / 2) - tickHeightOffset, 1.0f);
        mMatronGearDot->drawAt(g, center.getX() - (gearWidth / 2), center.getY() - (gearHeight / 2), 1.0f);

        g.setColour(juce::Colour(29u,26, 29u));
        jassert(rotaryStartAngle < rotaryEndAngle);

        auto sliderAngRad = juce::jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
        mMatronGearDot->setDrawableTransform( juce::AffineTransform().rotated(sliderAngRad,
                                                                           drawableCenter.getX(),
                                                                           drawableCenter.getY()));

        g.setFont(rswl->getTextHeight());
        auto text = rswl->getDisplayString();

    }
}

//==============================================================================
void RotarySliderWithLabels::paint(juce::Graphics &g)
{
    auto startAng = juce::degreesToRadians(180.f + 35.f);
    auto endAng = juce::degreesToRadians(180.f - 35.f) + juce::MathConstants<float>::twoPi;

    auto range = getRange();

    auto sliderBounds = getSliderBounds();

    getLookAndFeel().drawRotarySlider(g,
                                      sliderBounds.getX(),
                                      sliderBounds.getY(),
                                      sliderBounds.getWidth(),
                                      sliderBounds.getHeight(),
                                      juce::jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                      startAng,
                                      endAng,
                                      *this);

    auto center = sliderBounds.toFloat().getCentre();
    auto radius = sliderBounds.getWidth() * 0.5f;

    g.setColour(juce::Colour(30u, 39u, 25u));
    g.setFont((getTextHeight()));


    auto numChoices = labels.size();
    for( int i = 0; i < numChoices; ++i )
    {
        auto pos = labels[i].pos;
        jassert(0.f <= pos);
        jassert(pos <= 1.f);

        auto ang = juce::jmap(pos, 0.f, 1.f, startAng, endAng);

        auto c = center.getPointOnCircumference(radius + getTextHeight() * 0.5f + 1, ang);

        juce::Rectangle<float> r;
        auto str = labels[i].label;
        r.setSize(g.getCurrentFont().getStringWidth(str), getTextHeight());
        r.setCentre(c);
        r.setY(r.getY() + getTextHeight());

        g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
    }

}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

    size -= getTextHeight() * 2;
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(2);

    return r;
}

juce::String RotarySliderWithLabels::getDisplayString() const
{
    if( auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param) )
        return choiceParam->getCurrentChoiceName();

    juce::String str;
    bool addK = false;

    if( auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param) )
    {
        float val = getValue();

        if( val > 999.f )
        {
            val /= 1000.f; //1001 / 1000 = 1.001
            addK = true;
        }

        str = juce::String(val, (addK ? 2 : 0));
    }
    else
    {
        jassertfalse; //this shouldn't happen!
    }

    if( suffix.isNotEmpty() )
    {
        str << " ";
        if( addK )
            str << "k";

        str << suffix;
    }

    return str;
}

//==============================================================================
GrandMatronAudioProcessorEditor::GrandMatronAudioProcessorEditor (GrandMatronAudioProcessor& p)
    : AudioProcessorEditor (&p),
    mAudioProcessor(p),
    mLowPassFreqSlider(*mAudioProcessor.mApvts.getParameter("lowPass Freq"), "Hz"),
    mLowPassSlopeSlider(*mAudioProcessor.mApvts.getParameter("lowPass Slope"), "db/Oct"),
    mMatronHeaderUI (p),
    mMatronFooterUI (p),
    mLowPassFreqSliderAttachment(
        mAudioProcessor.mApvts, "lowPass Freq", mLowPassFreqSlider), mLowPassSlopeSliderAttachment(
          mAudioProcessor.mApvts, "lowPass Slope", mLowPassSlopeSlider)

{
    mLowPassFreqSlider.labels.add({0.f/3.f, "22Hz"});
    mLowPassFreqSlider.labels.add({1.f/3.f, "200"});
    mLowPassFreqSlider.labels.add({2.f/3.f, "2000"});
    mLowPassFreqSlider.labels.add({3.f/3.f, "20kHz"});

    mLowPassSlopeSlider.labels.add({0.f/6.f, "12dB"});
    mLowPassSlopeSlider.labels.add({1.f/6.f, "24"});
    mLowPassSlopeSlider.labels.add({2.f/6.f, "36"});
    mLowPassSlopeSlider.labels.add({3.f/6.f, "48"});
    mLowPassSlopeSlider.labels.add({4.f/6.f, "60"});
    mLowPassSlopeSlider.labels.add({5.f/6.f, "72"});
    mLowPassSlopeSlider.labels.add({6.f/6.f, "80dB"});

    addAndMakeVisible(mMatronHeaderUI);

    for( auto* comp : getComps() )
    {
        addAndMakeVisible(comp);
    }

    addAndMakeVisible(mMatronFooterUI);

    setSize (editorWidth, editorHeight);
}

GrandMatronAudioProcessorEditor::~GrandMatronAudioProcessorEditor()
{

}

//==============================================================================
void GrandMatronAudioProcessorEditor::paint(juce::Graphics &g)
{
    g.fillAll (juce::Colour (138, 190, 85));
}

void GrandMatronAudioProcessorEditor::resized()
    {
        auto area = getLocalBounds();
        mMatronHeaderUI.setBounds (area.removeFromTop(headerFooterHeight));

        mLowPassFreqSlider.setBounds(area.removeFromTop(cutoffRotaryKnobHeight) );

        mLowPassSlopeSlider.setBounds(area.removeFromTop(slopeChoiceRotaryKnobHeight));
        mMatronFooterUI.setBounds (area.removeFromTop(headerFooterHeight));

    }

    std::vector<juce::Component*> GrandMatronAudioProcessorEditor::getComps()
    {
        return
            {
                &mLowPassFreqSlider,
                &mLowPassSlopeSlider,
            };
    }
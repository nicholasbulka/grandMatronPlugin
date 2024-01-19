/*
==============================================================================
  This file contains the basic framework code for a JUCE plugin processor.
==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GrandMatronAudioProcessor::GrandMatronAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
  : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
  )
#endif
{
    juce::MemoryInputStream jsonStream (BinaryData::neural_net_weights_json, BinaryData::neural_net_weights_jsonSize, false);
    auto jsonInput = nlohmann::json::parse (jsonStream.readEntireStreamAsString().toStdString());
    inGainDbParam = mApvts.getRawParameterValue ("gain db");

    neuralNetT[0].parseJson (jsonInput);
    neuralNetT[1].parseJson (jsonInput);
}

GrandMatronAudioProcessor::~GrandMatronAudioProcessor()
{
}

//==============================================================================
const juce::String GrandMatronAudioProcessor::getName() const
{
  return JucePlugin_Name;
}

bool GrandMatronAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool GrandMatronAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool GrandMatronAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double GrandMatronAudioProcessor::getTailLengthSeconds() const
{
  return 0.0;
}

int GrandMatronAudioProcessor::getNumPrograms()
{
  return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
      // so this should be at least 1, even if you're not really implementing programs.
}

int GrandMatronAudioProcessor::getCurrentProgram()
{
  return 0;
}

void GrandMatronAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GrandMatronAudioProcessor::getProgramName (int index)
{
  return {};
}

void GrandMatronAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GrandMatronAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
  juce::dsp::ProcessSpec spec;

  spec.maximumBlockSize = samplesPerBlock;

  spec.numChannels = 1;

  spec.sampleRate = sampleRate;

  inputGain.prepare (spec);
  inputGain.setRampDurationSeconds (0.05);

  leftChain.prepare(spec);
  rightChain.prepare(spec);

  updateParams();

  leftChannelFifo.prepare(samplesPerBlock);
  rightChannelFifo.prepare(samplesPerBlock);

  spec.numChannels = getTotalNumOutputChannels();

  cleanBuffer.setSize(getTotalNumInputChannels(), samplesPerBlock);

  neuralNetT[0].reset();
  neuralNetT[1].reset();
}

void GrandMatronAudioProcessor::releaseResources()
{
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GrandMatronAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused (layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (//layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
      return false;

      // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
      return false;
#endif

  return true;
#endif
}
#endif

void GrandMatronAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels  = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
      buffer.clear (i, 0, buffer.getNumSamples());

  updateParams();

  juce::AudioBuffer<float> tempCleanBuffer(cleanBuffer.getArrayOfWritePointers(), buffer.getNumChannels(), buffer.getNumSamples());
  for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
  {
      tempCleanBuffer.copyFrom (ch, 0, buffer, ch, 0, buffer.getNumSamples());
  }
  // now use tmpBuffer
  juce::dsp::AudioBlock<float> block(buffer);
  juce::dsp::ProcessContextReplacing<float> monoContext (block);
  inputGain.process (monoContext);

  auto leftBlock = block.getSingleChannelBlock(0);
  auto rightBlock = block.getSingleChannelBlock(1);

  juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
  juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

  leftChain.process(leftContext);
  rightChain.process(rightContext);

  leftChannelFifo.update(buffer);
  rightChannelFifo.update(buffer);

  //@TODO: parameterize this.
  float networkFilterBufferPercentage = 0.99f;
  float digitalFilterBufferPercentage = 1.f - networkFilterBufferPercentage;

  for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
  {
      auto* x1 = tempCleanBuffer.getWritePointer(ch);
      auto* x2 = buffer.getWritePointer (ch);
      for (int n = 0; n < buffer.getNumSamples(); ++n)
      {
          float input[] = { x1[n], x2[n] };
          x2[n] = networkFilterBufferPercentage * (neuralNetT[ch].forward(input)) + digitalFilterBufferPercentage * x2[n];
      }
  }
}

//==============================================================================
bool GrandMatronAudioProcessor::hasEditor() const
{
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GrandMatronAudioProcessor::createEditor()
{
  return new GrandMatronAudioProcessorEditor (*this);
}

//==============================================================================
void GrandMatronAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.

  juce::MemoryOutputStream mos(destData, true);
  mApvts.state.writeToStream(mos);
}

void GrandMatronAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
  // You should use this method to restore your parameters from this memory block,
  // whose contents will have been created by the getStateInformation() call.
  auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
  if( tree.isValid() )
  {
      mApvts.replaceState(tree);
      updateParams();
  }
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
  ChainSettings settings;

  settings.lowPassFreq = apvts.getRawParameterValue("lowPass Freq")->load();
  settings.lowPassSlope = static_cast<Slope>(apvts.getRawParameterValue("lowPass Slope")->load());
  settings.lowPassBypassed = apvts.getRawParameterValue("lowPass Bypassed")->load() > 0.5f;

  return settings;
}

void updateCoefficients(Coefficients &old, const Coefficients &replacements)
{
  *old = *replacements;
}

void GrandMatronAudioProcessor::updateLowPassFilters(const ChainSettings &chainSettings)
{
  auto lowPassCoefficients = makeLowPassFilter(chainSettings, getSampleRate());

  auto& leftLowPass = leftChain.get<ChainPositions::lowPass>();
  auto& rightLowPass = rightChain.get<ChainPositions::lowPass>();

  leftChain.setBypassed<ChainPositions::lowPass>(chainSettings.lowPassBypassed);
  rightChain.setBypassed<ChainPositions::lowPass>(chainSettings.lowPassBypassed);

  updateCutFilter(leftLowPass, lowPassCoefficients, chainSettings.lowPassSlope);
  updateCutFilter(rightLowPass, lowPassCoefficients, chainSettings.lowPassSlope);

}

void GrandMatronAudioProcessor::updateParams()
{
  auto chainSettings = getChainSettings(mApvts);
  updateLowPassFilters(chainSettings);
  inputGain.setGainDecibels (inGainDbParam->load() + getAdditionalInputGain());

}

juce::AudioProcessorValueTreeState::ParameterLayout GrandMatronAudioProcessor::createParameterLayout()
{
  juce::AudioProcessorValueTreeState::ParameterLayout layout;

  juce::ParameterID lowPassFreqId = {"lowPass Freq", 2};

  layout.add(std::make_unique<juce::AudioParameterFloat>(lowPassFreqId,
                                                         "lowPass Freq",
                                                         juce::NormalisableRange<float>(20.f,
                                                                                        20000.f,
                                                                                        linToLogLambda,
                                                                                        logToLinLambda),
                                                         20000.f));

  juce::StringArray stringArray;
  for( int i = 0; i < 7; ++i )
  {
      juce::String str;
      str << (12 + i*12);
      str << " db/Oct";
      stringArray.add(str);
  }

  juce::ParameterID lowPassSlopeId = {"lowPass Slope", 2};
  juce::ParameterID lowPassBypassedId = {"lowPass Bypassed", 2};
  juce::ParameterID gainId = {"gain db", 2};

  layout.add(std::make_unique<juce::AudioParameterFloat> (gainId, "gain db", -12.0f, 12.0f, 0.0f));
  layout.add(std::make_unique<juce::AudioParameterChoice>(lowPassSlopeId, "lowPass Slope", stringArray, 0));

  layout.add(std::make_unique<juce::AudioParameterBool>(lowPassBypassedId, "lowPass Bypassed", false));
  return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return new GrandMatronAudioProcessor();
}
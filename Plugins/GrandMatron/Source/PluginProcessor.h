/*
==============================================================================
  This file contains the basic framework code for a JUCE plugin processor.
==============================================================================
*/

#pragma once

#include <shared_plugin_helpers/shared_plugin_helpers.h>
#include <array>
#include "BinaryData.h"

template<typename T>
struct Fifo
{
  void prepare(int numChannels, int numSamples)
  {
      static_assert( std::is_same_v<T, juce::AudioBuffer<float>>,
                    "prepare(numChannels, numSamples) should only be used when the Fifo is holding juce::AudioBuffer<float>");
      for( auto& buffer : buffers)
      {
          buffer.setSize(numChannels,
                         numSamples,
                         false,   //clear everything?
                         true,    //including the extra space?
                         true);   //avoid reallocating if you can?
          buffer.clear();
      }
  }

  void prepare(size_t numElements)
  {
      static_assert( std::is_same_v<T, std::vector<float>>,
                    "prepare(numElements) should only be used when the Fifo is holding std::vector<float>");
      for( auto& buffer : buffers )
      {
          buffer.clear();
          buffer.resize(numElements, 0);
      }
  }

  bool push(const T& t)
  {
      auto write = fifo.write(1);
      if( write.blockSize1 > 0 )
      {
          buffers[write.startIndex1] = t;
          return true;
      }

      return false;
  }

  bool pull(T& t)
  {
      auto read = fifo.read(1);
      if( read.blockSize1 > 0 )
      {
          t = buffers[read.startIndex1];
          return true;
      }

      return false;
  }

  int getNumAvailableForReading() const
  {
      return fifo.getNumReady();
  }
private:
  static constexpr int Capacity = 30;
  std::array<T, Capacity> buffers;
  juce::AbstractFifo fifo {Capacity};
};

enum Channel
{
  Right, //effectively 0
  Left //effectively 1
};

template<typename BlockType>
struct SingleChannelSampleFifo
{
  SingleChannelSampleFifo(Channel ch) : channelToUse(ch)
  {
      prepared.set(false);
  }

  void update(const BlockType& buffer)
  {
      jassert(prepared.get());
      jassert(buffer.getNumChannels() > channelToUse );
      auto* channelPtr = buffer.getReadPointer(channelToUse);

      for( int i = 0; i < buffer.getNumSamples(); ++i )
      {
          pushNextSampleIntoFifo(channelPtr[i]);
      }
  }

  void prepare(int bufferSize)
  {
      prepared.set(false);
      size.set(bufferSize);

      bufferToFill.setSize(1,             //channel
                           bufferSize,    //num samples
                           false,         //keepExistingContent
                           true,          //clear extra space
                           true);         //avoid reallocating
      audioBufferFifo.prepare(1, bufferSize);
      fifoIndex = 0;
      prepared.set(true);
  }
  //==============================================================================
  int getNumCompleteBuffersAvailable() const { return audioBufferFifo.getNumAvailableForReading(); }
  bool isPrepared() const { return prepared.get(); }
  int getSize() const { return size.get(); }
  //==============================================================================
  bool getAudioBuffer(BlockType& buf) { return audioBufferFifo.pull(buf); }
private:
  Channel channelToUse;
  int fifoIndex = 0;
  Fifo<BlockType> audioBufferFifo;
  BlockType bufferToFill;
  juce::Atomic<bool> prepared = false;
  juce::Atomic<int> size = 0;

  void pushNextSampleIntoFifo(float sample)
  {
      if (fifoIndex == bufferToFill.getNumSamples())
      {
          auto ok = audioBufferFifo.push(bufferToFill);

          juce::ignoreUnused(ok);

          fifoIndex = 0;
      }

      bufferToFill.setSample(0, fifoIndex, sample);
      ++fifoIndex;
  }
};

enum Slope
{
  Slope_12,
  Slope_24,
  Slope_36,
  Slope_48,
    Slope_60,
    Slope_72,
    Slope_84,
};

struct ChainSettings
{
  float highPassFreq { 0 }, lowPassFreq { 0 };

  Slope highPassSlope { Slope::Slope_12 }, lowPassSlope { Slope::Slope_12 };

  bool highPassBypassed { false }, peakBypassed { false }, lowPassBypassed { false };
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

using Filter = juce::dsp::IIR::Filter<float>;

using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter, Filter, Filter, Filter>;

using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

enum ChainPositions
{
  lowPass
};

using Coefficients = Filter::CoefficientsPtr;
void updateCoefficients(Coefficients& old, const Coefficients& replacements);

Coefficients makePeakFilter(const ChainSettings& chainSettings, double sampleRate);

template<int Index, typename ChainType, typename CoefficientType>
void update(ChainType& chain, const CoefficientType& coefficients)
{
  updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]);
  chain.template setBypassed<Index>(false);
}

template<typename ChainType, typename CoefficientType>
void updateCutFilter(ChainType& chain,
                   const CoefficientType& coefficients,
                   const Slope& slope)
{
  chain.template setBypassed<0>(true);
  chain.template setBypassed<1>(true);
  chain.template setBypassed<2>(true);
  chain.template setBypassed<3>(true);
  chain.template setBypassed<4>(true);
  chain.template setBypassed<5>(true);
  chain.template setBypassed<6>(true);

  switch( slope )
  {
      case Slope_84:
      {
          update<6>(chain, coefficients);
      }
      case Slope_72:
      {
          update<5>(chain, coefficients);
      }
      case Slope_60:
      {
          update<4>(chain, coefficients);
      }
      case Slope_48:
      {
          update<3>(chain, coefficients);
      }
      case Slope_36:
      {
          update<2>(chain, coefficients);
      }
      case Slope_24:
      {
          update<1>(chain, coefficients);
      }
      case Slope_12:
      {
          update<0>(chain, coefficients);
      }
  }
}

inline auto linToLogLambda = [](float min, float max, float linVal) {
    /*
     * Wolfram alpha curve fitting:
     * exponential fit {.01, 20,},{.33,272},{.5,660},{.66,2000},{.75,3600},{1.0,20000}
     * */
    auto result = 21.8085 * exp(6.82108 * linVal);
    return result;
};

inline auto logToLinLambda = [](float min, float max, float logVal) {
    /*
     * Wolfram alpha curve fitting:
     * log fit {20,.01},{272,.33},{660,.5},{2000,.66},{3600,.75},{20000,1.0}
     * */
    auto result = 0.145229 * log(0.0465104 * logVal);
    return result;
};

inline auto makeLowPassFilter(const ChainSettings& chainSettings, double sampleRate )
{
  return juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.lowPassFreq,
                                                                                    sampleRate,
                                                                                    2 * (chainSettings.lowPassSlope + 1));
}
//==============================================================================
/**
*/
class GrandMatronAudioProcessor  : public juce::AudioProcessor
{
public:
  //==============================================================================
  GrandMatronAudioProcessor();
  ~GrandMatronAudioProcessor() override;

  //==============================================================================
  void prepareToPlay (double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif

  void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  //==============================================================================
  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram (int index) override;
  const juce::String getProgramName (int index) override;
  void changeProgramName (int index, const juce::String& newName) override;

  //==============================================================================
  void getStateInformation (juce::MemoryBlock& destData) override;
  void setStateInformation (const void* data, int sizeInBytes) override;

  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
  juce::AudioProcessorValueTreeState mApvts {*this, nullptr, "Parameters", createParameterLayout()};

  using BlockType = juce::AudioBuffer<float>;
  SingleChannelSampleFifo<BlockType> leftChannelFifo { Channel::Left };
  SingleChannelSampleFifo<BlockType> rightChannelFifo { Channel::Right };
  private:
  MonoChain leftChain, rightChain;

  void updateLowPassFilters(const ChainSettings& chainSettings);

  void updateParams();

  juce::AudioBuffer<float> cleanBuffer;

  float getAdditionalInputGain(){ return 5.f;}

  //model defined at compile-time
  static const int NN_INPUT_SIZE = 2;
  static const int NN_OUTPUT_SIZE = 1;
  alignas (RTNEURAL_DEFAULT_ALIGNMENT) float mInput[NN_INPUT_SIZE];

  RTNeural::ModelT<float, NN_INPUT_SIZE, NN_OUTPUT_SIZE,
                   RTNeural::GRULayerT<float, 2, 2>,
                   RTNeural::GRULayerT<float, 2, 2>,
                   RTNeural::GRULayerT<float, 2, 2>,
                   RTNeural::GRULayerT<float, 2, 2>,
                   RTNeural::GRULayerT<float, 2, 2>,
                   RTNeural::GRULayerT<float, 2, 2>,
                   RTNeural::GRULayerT<float, 2, 2>,
                   RTNeural::GRULayerT<float, 2, 2>,
                   RTNeural::GRULayerT<float, 2, 2>,
                   RTNeural::GRULayerT<float, 2, 8>,
                   RTNeural::DenseT<float, 8, NN_OUTPUT_SIZE>
                   > neuralNetT[2];

  std::atomic<float>* inGainDbParam = nullptr;
  juce::dsp::Gain<float> inputGain;
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrandMatronAudioProcessor)
};
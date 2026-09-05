/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Utils.h"
#include "JucePluginDefines.h"


class MyPlugin;
class JX11AudioProcessorEditor;

//Per la gestione dei valori presi da JSON e gestiti all'interno del codice
#include <unordered_map>
#include <variant>
#include <string_view>
#include <iostream>

// Per-block copy of JUCE transport/position information.  The JUCE
// AudioPlayHead and PositionInfo objects remain private to PluginProcessor;
// this is the boundary that can later be consumed by generated DSP code.
struct HostTransportInfo
{
  static constexpr double fallbackBpm = 120.0;

  bool playHeadAvailable = false;
  bool positionAvailable = false;

  juce::Optional<double> bpm;
  juce::Optional<double> timeInSeconds;
  juce::Optional<double> ppqPosition;
  bool isPlaying = false;
  juce::Optional<int64_t> timeInSamples;
  juce::Optional<juce::AudioPlayHead::TimeSignature> timeSignature;
  juce::Optional<juce::AudioPlayHead::LoopPoints> loopPoints;
  juce::Optional<int64_t> barCount;
  juce::Optional<double> ppqPositionOfLastBarStart;
  juce::Optional<juce::AudioPlayHead::FrameRate> frameRate;
  juce::Optional<double> editOriginTime;
  juce::Optional<uint64_t> hostTimeNs;
  bool isRecording = false;
  bool isLooping = false;

  double bpmOrFallback() const noexcept
  {
    return bpm.orFallback (fallbackBpm);
  }
};


//==============================================================================
/**
 */
class JX11AudioProcessor : public juce::AudioProcessor //, private juce::ValueTree::Listener
{
public:
  //==============================================================================
  JX11AudioProcessor();
  ~JX11AudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  void reset() override;

  // In questo modo blocco il supporto a mono e stereo
  // e imposto il layout mono o stereo per input e output
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override {
    auto inputLayout  = layouts.getMainInputChannelSet();
    auto outputLayout = layouts.getMainOutputChannelSet();

    // Only allow mono or stereo, and input must match output
    if ((inputLayout == juce::AudioChannelSet::mono() &&
         outputLayout == juce::AudioChannelSet::mono()) ||
        (inputLayout == juce::AudioChannelSet::stereo() &&
         outputLayout == juce::AudioChannelSet::stereo()))
      return true;
    else
      return false;
  }

  /// DSP_RUNTIME_MEMBERS START

  std::atomic<float> meterInPeak { 0.0f };
  std::atomic<float> meterOutPeak { 0.0f };

  // NUOVO BUFFER THREAD-SAFE PER LO SCOPE
  std::array<std::atomic<float>, 128> scopeFifo {};
  std::atomic<int> scopeWriteIdx { 0 };

  /// DSP_RUNTIME_MEMBERS END

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  //==============================================================================
  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override;
  JX11AudioProcessorEditor *theAudioProcessorEditor;

  // I parametri a run time
  juce::AudioProcessorValueTreeState parameters;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String &newName) override;

  //==============================================================================
  DrawingUtils drawingUtils;
  std::atomic<bool> windowSizeNeedsUpdate { false };

  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;


private:
  juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

  std::atomic<bool> parametersChanged{false};

  juce::AudioBuffer<float> dryBuffer;

  void update();
  void createPrograms();

  void handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2);

public:
  // Codice plugin: Qui ci va la classe che implementa il codice del plugin
  std::unique_ptr<MyPlugin> myplugin;

  float value_info_sampleRate;
  float value_info_inverseSampleRate;
  int value_info_max_samplesPerBlock;
  int value_info_totalNumInputChannels;
  int value_info_totalNumOutputChannels;

  HostTransportInfo hostTransportInfo;

  // Temporary compatibility mirrors for existing generated/developer code.
  // They are written only from hostTransportInfo in processBlock and are not
  // a second source of transport state.
  juce::Optional<double> value_info_BPM;
  juce::Optional<double> value_info_timeInSeconds;
  juce::Optional<double> value_info_ppqPosition;
  bool value_info_isPlaying = false;
  juce::Optional<int64_t> value_info_timeInSamples;
  juce::Optional<juce::AudioPlayHead::TimeSignature> value_info_timeSignature;
  juce::Optional<juce::AudioPlayHead::LoopPoints> value_info_loopPoints;
  juce::Optional<int64_t> value_info_barCount;
  juce::Optional<double> value_info_ppqPositionOfLastBarStart;
  juce::Optional<juce::AudioPlayHead::FrameRate> value_info_frameRate;
  juce::Optional<double> value_info_editOriginTime;
  juce::Optional<uint64_t> value_info_hostTimeNs;
  bool value_info_isRecording = false;
  bool value_info_isLooping = false;

  ///OVERSAMPLING_PPH START

  // std::unique_ptr<juce::dsp::Oversampling<float>> oversampling;
  // size_t oversampling_factor=2;

std::unique_ptr<juce::dsp::Oversampling<float>> oversampling2x;
std::unique_ptr<juce::dsp::Oversampling<float>> oversampling4x;
std::unique_ptr<juce::dsp::Oversampling<float>> oversampling8x;

  ///OVERSAMPLING_PPH END


  // qui ci vanno le dichiarazioni dei parametri che sono in comune tra il codice e l'editor
  ///DPARAMS START

// std::atomic<float>* param_inputdB = nullptr;
// 	float value_inputdB;

std::atomic<float>* param_volumedB = nullptr;
	float value_volumedB;

std::atomic<float>* param_outVol = nullptr;
	float value_outVol;
	float value_outputdB=0.0f; //solo per poter lavorare senza errori

std::atomic<float>* param_outSqueeze = nullptr;
	float value_outSqueeze;

std::atomic<float>* param_stepSld = nullptr;
	float value_stepSld;

std::atomic<float>* param_RotSld = nullptr;
	float value_RotSld;

std::atomic<float>* param_BtnOnOff = nullptr;
	float value_BtnOnOff;

std::atomic<float>* param_BtnEnDis = nullptr;
	float value_BtnEnDis;

std::atomic<float>* param_Bypass = nullptr;
	float value_Bypass;

std::atomic<float>* param_wetdry = nullptr;
  float value_wetdry=1.0f;

///DPARAMS END

//==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JX11AudioProcessor)
};

#include <juce_core/juce_core.h>


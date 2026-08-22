/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "MyPlugin.h"

#include "Utils.h"
#include <numbers>

using namespace std;

//==============================================================================
JX11AudioProcessor::JX11AudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
                                           AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                                                              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                                                              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                                                              )
#endif
                                           ,
                                           parameters(*this, nullptr, juce::Identifier("PARAMETERS"), createParameterLayout())
{
    // Codice plugin: inizializza a null!
    myplugin.reset();

    // Dove dichiaro i collegamenti con i parametri per prenderli nella process block
    /// GETPARAMS START

    param_inputdB = parameters.getRawParameterValue("LogVSliderID");
    param_volumedB = parameters.getRawParameterValue("LogSliderID");
    param_outVol = parameters.getRawParameterValue("LinearSliderID");
    param_outSqueeze = parameters.getRawParameterValue("LinearVSliderID");
    param_stepSld = parameters.getRawParameterValue("LinearVSliderSteppedID");
    param_RotSld = parameters.getRawParameterValue("RotarySliderID");
    param_BtnOnOff = parameters.getRawParameterValue("ToggleButtonID");
    param_BtnEnDis = parameters.getRawParameterValue("SwitchID");
    param_Bypass = parameters.getRawParameterValue("AFBypass");

    /// GETPARAMS END
}

JX11AudioProcessor::~JX11AudioProcessor()
{
}

//==============================================================================
const juce::String JX11AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JX11AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool JX11AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool JX11AudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double JX11AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JX11AudioProcessor::getNumPrograms()
{
    return 0;
}

int JX11AudioProcessor::getCurrentProgram()
{
    return 0;
}

void JX11AudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
    reset();
}

const juce::String JX11AudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return "";
}

void JX11AudioProcessor::changeProgramName(int index, const juce::String &newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void JX11AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    this->value_info_sampleRate = sampleRate;
    this->value_info_max_samplesPerBlock = samplesPerBlock;

    // Pre-alloca la memoria. 2 canali e dimensione massima del blocco.
    dryBuffer.setSize(getTotalNumInputChannels(), samplesPerBlock);

    myplugin = std::make_unique<MyPlugin>(this);
    if (myplugin != nullptr)
    {
        myplugin->prepare(
            sampleRate,
            samplesPerBlock,
            juce::jmax(
                getTotalNumInputChannels(),
                getTotalNumOutputChannels()));
    }

    /// OVERSAMPLING_PPC START

    /// OVERSAMPLING_PPC END

}

void JX11AudioProcessor::releaseResources()
{
    myplugin.reset();

    /// OVERSAMPLING_PPCRR START
    /// OVERSAMPLING_PPCRR END
}

void JX11AudioProcessor::reset()
{
    if (myplugin != nullptr)
        myplugin->reset();
}

void JX11AudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    // mono o stereo, null'altro!
    jassert(buffer.getNumChannels() == 1 || buffer.getNumChannels() == 2);

    if (auto *playHead = getPlayHead())
    {
        if (auto posInfo = playHead->getPosition())
        {
            value_info_BPM = posInfo->getBpm();                                             // Returns the bpm, if available
            value_info_timeInSeconds = posInfo->getTimeInSeconds();                         // Returns the number of seconds that have elapsed.
            value_info_ppqPosition = posInfo->getPpqPosition();                             // The current playback position in "Pulses Per Quarter Note"
            value_info_isPlaying = posInfo->getIsPlaying();                                 // Whether the host is currently playing
            value_info_timeInSamples = posInfo->getTimeInSamples();                         // the number of samples that have elapsed.
            value_info_timeSignature = posInfo->getTimeSignature();                         // The time signature, if available
            value_info_loopPoints = posInfo->getLoopPoints();                               // The loop points, if available
            value_info_barCount = posInfo->getBarCount();                                   // The number of bars in the current loop, if available
            value_info_ppqPositionOfLastBarStart = posInfo->getPpqPositionOfLastBarStart(); // The position of the start of the last bar, in units of quarter-notes
            value_info_frameRate = posInfo->getFrameRate();                                 // The video frame rate, if available
            value_info_editOriginTime = posInfo->getEditOriginTime();                       // The origin time of the current edit, in seconds
            value_info_hostTimeNs = posInfo->getHostTimeNs();                               // The current host time in nanoseconds
            value_info_isRecording = posInfo->getIsRecording();                             // Whether the host is currently recording
            value_info_isLooping = posInfo->getIsLooping();                                 // Whether the host is currently looping
        }
    }

    value_info_totalNumInputChannels = buffer.getNumChannels();
    value_info_totalNumOutputChannels = getTotalNumOutputChannels();
    value_info_sampleRate = float(getSampleRate());
    value_info_inverseSampleRate = 1.0f / value_info_sampleRate;
    value_info_max_samplesPerBlock = buffer.getNumSamples();

    // Qui i valori per questo ciclo dei parametri!
    /// VALUEPARAMS START

    value_inputdB = param_inputdB->load();

    value_volumedB = param_volumedB->load();

    value_outVol = param_outVol->load();

    value_outSqueeze = param_outSqueeze->load();

    value_stepSld = param_stepSld->load();

    value_RotSld = param_RotSld->load();

    value_BtnOnOff = param_BtnOnOff->load();

    value_BtnEnDis = param_BtnEnDis->load();

    value_Bypass = param_Bypass->load();

    /// VALUEPARAMS END

    
    /// PROCESS START


    // Se disabilitato il plugin prende solo volume input e output
    //  input buffers and output buffers are often the same
    //  In this case if you does not want to modify, simply return!
    //  if you want to modify, you can do someting like
    //  buffer.applyGain(0.5); // dimezza l'ampiezza
    //  buffer.applyGainRamp(0, 0, processor->samplesPerBlock)
    //  only if the DAW makes some routing the buffers are different
    //  and in such case the output buffers are different from the input ones
    //  and you have to copy and move data

    // Se il plugin è disabilitato, non facciamo nulla
    if (false && value_Bypass >= 0.5)
    {
        //buffer.applyGain(value_inputdB);
        buffer.applyGain(value_outputGain);
        return; // Non fare nulla se il plugin è bypassato
    }

    // Altrimenti prosegue con la sua strada, anche complessa.
    // Nota che si aspetta ci siano SEMPRE inputDb e outputDb e lo gestisce direttamente
    // il nostro codice
    buffer.applyGain(1.0); //value_inputdB);
    
    // 1. LEGGI IL PICCO IN INGRESSO (dopo il gain iniziale)
    meterInPeak.store(buffer.getMagnitude(0, 0, buffer.getNumSamples()));


    /// WETDRY_PPC_PREFIX START

    // Salva una dry copia
    for (int i = 0; i < buffer.getNumChannels(); ++i)
        dryBuffer.copyFrom(i, 0, buffer.getReadPointer(i), buffer.getNumSamples());

    /// WETDRY_PPC_PREFIX END

    /// OVERSAMPLING_PPCPB START
    myplugin->processAudio(buffer, 1);

    /// OVERSAMPLING_PPCPB END

    /// WETDRY_PPC_POSTFIX START

    if (false && value_wetdry < 1.0f) 
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* wet = buffer.getWritePointer(ch);
            const auto* dry = dryBuffer.getReadPointer(ch);
            int numSamples = buffer.getNumSamples();

            // Formula: Out = Dry * (1 - mix) + Wet * mix
            // Usiamo buffer.applyGain (che è vettoriale) sul Wet
            // E buffer.addFrom (vettoriale) per sommare il Dry

            // Nota: Questo richiede una logica leggermente diversa per non distruggere il buffer
            // La via più semplice e veloce con JUCE è:
            
            juce::FloatVectorOperations::multiply(wet, value_wetdry, numSamples);
            juce::FloatVectorOperations::addWithMultiply(wet, dry, 1.0f - value_wetdry, numSamples);
        }
    }

    /// WETDRY_PPC_POSTFIX END

    buffer.applyGain(1.0); //value_outputdB);
    meterOutPeak.store(buffer.getMagnitude(0, 0, buffer.getNumSamples()));

// 3. INVIA I DATI ALL'OSCILLOSCOPIO (Senza toccare l'Editor!)
    const float* data = buffer.getReadPointer(0);
    int numSamples = buffer.getNumSamples();
    int step = juce::jmax(1, numSamples / 128);
    int idx = scopeWriteIdx.load(std::memory_order_relaxed);

    for (int i = 0; i < numSamples; i += step) {
        scopeFifo[idx].store(data[i], std::memory_order_relaxed);
        idx = (idx + 1) % 128;
    }
    
    scopeWriteIdx.store(idx, std::memory_order_relaxed);

    /// PROCESS END



}

void JX11AudioProcessor::update()
{
    value_info_sampleRate = float(getSampleRate());
    value_info_inverseSampleRate = 1.0f / value_info_sampleRate;
}

void JX11AudioProcessor::handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2)
{
    juce::ignoreUnused(data0, data1, data2);
}

//==============================================================================
bool JX11AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *JX11AudioProcessor::createEditor()
{
    auto theAudioProcessorEditor = new JX11AudioProcessorEditor(*this);
    return theAudioProcessorEditor;
}

//==============================================================================

static const juce::Identifier pluginTag = JucePlugin_Name; // "PLUGIN";

void JX11AudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    auto xml = std::make_unique<juce::XmlElement>(pluginTag);
    std::unique_ptr<juce::XmlElement> parametersXML(parameters.copyState().createXml());
    xml->addChildElement(parametersXML.release());

    xml->setAttribute("g_windowWidth", drawingUtils.currentScreenWidth);
    xml->setAttribute("g_windowHeight", drawingUtils.currentScreenHeight);

    copyXmlToBinary(*xml, destData);
}


void JX11AudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    
    if (xml.get() != nullptr && xml->hasTagName(pluginTag))
    {
        // 1. Estrazione dati (Background Thread)
        const int newWidth = xml->getIntAttribute("g_windowWidth", drawingUtils.standardScreenWidth);
        const int newHeight = xml->getIntAttribute("g_windowHeight", drawingUtils.standardScreenHeight);
        
        juce::ValueTree newParamsState;
        if (auto *pXML = xml->getChildByName(parameters.state.getType()))
            newParamsState = juce::ValueTree::fromXml(*pXML);

        // 2. Prepara il SafePointer all'editor
        // SafePointer diventa automaticamente null se l'editor viene distrutto
        juce::Component::SafePointer<JX11AudioProcessorEditor> safeEditor (dynamic_cast<JX11AudioProcessorEditor*>(getActiveEditor()));

        // 3. Sposta tutto sul Message Thread
        juce::MessageManager::callAsync([this, safeEditor, newParamsState, newWidth, newHeight]()
        {
            // A. Aggiorna parametri (Sempre safe qui)
            if (newParamsState.isValid())
                parameters.replaceState(newParamsState);

            // B. Aggiorna variabili nel processor
            this->drawingUtils.currentScreenWidth = newWidth;
            this->drawingUtils.currentScreenHeight = newHeight;

            // C. Aggiorna l'Editor DIRETTAMENTE (Se esiste ancora)
            if (safeEditor != nullptr)
            {
                // Passiamo le dimensioni all'editor che deciderà cosa fare
                safeEditor->resizeEditorFromProcessor(newWidth, newHeight);
            }
        });
    }
}

//==============================================================================
void JX11AudioProcessor::createPrograms()
{
}

/********************
 ******************** L A  G E S T I O N E  D E L L ' I N T E R F A C C I A
 ********************/

juce::AudioProcessorValueTreeState::ParameterLayout JX11AudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    /// PARAMS START

    auto pverticalLogSlider_0001 = new juce::AudioParameterFloat(
        "LogVSliderID", "Logarithmic VSlider modificato",
        juce::NormalisableRange<float>(
            juce::Decibels::decibelsToGain(-60.0f), // Convert dB to linear min gain
            juce::Decibels::decibelsToGain(12.0f),  // Convert dB to linear max gain
            [](float start, float end, float normalised) -> float
            {
                juce::ignoreUnused(start, end);
                return juce::Decibels::decibelsToGain(
                    juce::jmap(std::pow(normalised, 0.5f), -60.0f, 12.0f));
            },
            [](float start, float end, float value) -> float
            {
                juce::ignoreUnused(start, end);
                return std::pow(
                    juce::jmap(juce::Decibels::gainToDecibels(value), -60.0f, 12.0f, 0.0f, 1.0f), 2.0f);
            }),
        juce::Decibels::decibelsToGain(0.0f), // Default value at 0 dB (1.0 gain)
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int)
                                         { return juce::String(juce::Decibels::gainToDecibels(value), 2) + " dB"; })
            .withValueFromStringFunction([](const juce::String &text)
                                         { return juce::Decibels::decibelsToGain(text.getFloatValue()); }));

    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(pverticalLogSlider_0001));

    auto phorizontalLogSlider_0001 = new juce::AudioParameterFloat(
        "LogSliderID", "Logarithmic Slider",
        juce::NormalisableRange<float>(
            juce::Decibels::decibelsToGain(-60.0f), // Convert dB to linear min gain
            juce::Decibels::decibelsToGain(12.0f),  // Convert dB to linear max gain
            [](float start, float end, float normalised) -> float
            {
                juce::ignoreUnused(start, end);
                return juce::Decibels::decibelsToGain(
                    juce::jmap(std::pow(normalised, 0.5f), -60.0f, 12.0f));
            },
            [](float start, float end, float value) -> float
            {
                juce::ignoreUnused(start, end);
                return std::pow(
                    juce::jmap(juce::Decibels::gainToDecibels(value), -60.0f, 12.0f, 0.0f, 1.0f), 2.0f);
            }),
        juce::Decibels::decibelsToGain(0.0f), // Default value at 0 dB (1.0 gain)
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int)
                                         { return juce::String(juce::Decibels::gainToDecibels(value), 2) + " dB"; })
            .withValueFromStringFunction([](const juce::String &text)
                                         { return juce::Decibels::decibelsToGain(text.getFloatValue()); }));

    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(phorizontalLogSlider_0001));

    auto phorizontalLinearSlider_0001 = new juce::AudioParameterFloat("LinearSliderID", "Linear Slider", 0, 100, 50);
    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(phorizontalLinearSlider_0001));

    auto pverticalLinearSlider_0001 = new juce::AudioParameterFloat("LinearVSliderID", "Linear VSlider", 0, 100, 50);
    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(pverticalLinearSlider_0001));

    auto pverticalSteppedSlider_0001 = new juce::AudioParameterChoice("LinearVSliderSteppedID", "Linear VSlider Stepped",
                                                                      juce::StringArray{"uno", "due", "tre", "quattro", "cinque", "sei", "sette", "otto", "nove", "dieci"}, 5);
    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(pverticalSteppedSlider_0001));

    auto protarySlider_0001 = new juce::AudioParameterFloat("RotarySliderID", "Rotary Slider", 0, 100, 50);
    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(protarySlider_0001));

    auto pmaterialButton_0001 = new juce::AudioParameterBool("ToggleButtonID", "Toggle Button", false);
    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(pmaterialButton_0001));

    auto pmaterialToggle_0001 = new juce::AudioParameterBool("SwitchID", "Switch", true);
    params.push_back(std::unique_ptr<juce::RangedAudioParameter>(pmaterialToggle_0001));

    // Il bottone di bypass del plugin!
    params.push_back(std::make_unique<juce::AudioParameterBool>("AFBypass", "Bypass", false));

    /// PARAMS END

    return {params.begin(), params.end()};
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new JX11AudioProcessor();
}


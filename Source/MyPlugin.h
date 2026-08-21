#pragma once

// La classe per gestire tutti i dati del vibrato e per modificare la forma d'onda
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Oscillator.h"
#include "Utils.h"
#include "PluginEditor.h"
#include "Synth.h"

class MyPlugin
{
public:
    MyPlugin(JX11AudioProcessor *apk);
    ~MyPlugin();

    void ButtonCallback(int num, juce::String name) {
        realPlugin1x->ButtonCallback(num, name);
        realPlugin2x->ButtonCallback(num, name);
        realPlugin4x->ButtonCallback(num, name);
        realPlugin8x->ButtonCallback(num, name);
    }
    void prepare(
    double sampleRate,
    int samplesPerBlock,
    int numChannels);

void render(
    juce::AudioBuffer<float>& buffer,
    int oversamplingFactor);

void render(
    juce::dsp::AudioBlock<float>& buffer,
    int oversamplingFactor);


    JX11AudioProcessor *processor=nullptr;

    std::unique_ptr<RealPlugin> realPlugin1x;
    std::unique_ptr<RealPlugin> realPlugin2x;
    std::unique_ptr<RealPlugin> realPlugin4x;
    std::unique_ptr<RealPlugin> realPlugin8x;

    /// FFT_MYPLUGIN_MEMBERS START

    /// FFT_MYPLUGIN_MEMBERS END

};

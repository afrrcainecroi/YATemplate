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
        realPlugin->ButtonCallback(num, name);
    }

    void render(juce::AudioBuffer<float> &buffer);

    //Resampling, when activated
    void render(juce::dsp::AudioBlock<float> &buffer);

    JX11AudioProcessor *processor=nullptr;
    std::unique_ptr<RealPlugin> realPlugin;
};

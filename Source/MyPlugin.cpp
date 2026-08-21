// La classe per gestire tutti i dati del vibrato e per modificare la forma d'onda
#include "MyPlugin.h"

// Chiamato dalla PrepareToPlay. Tieni presente che sampleRate e samplesPerBlock possono cambiare ad ogni pacco dati
MyPlugin::MyPlugin(JX11AudioProcessor *ap)
    : processor(ap)
{
    jassert(processor->value_info_sampleRate >0 && processor->value_info_max_samplesPerBlock > 0);

    //Qui inizializza il plugin presente in Synth.cpp/Synth.h
    //realPlugin = std::make_unique<RealPlugin>(processor);

    /// MYPLUGIN_FFT_INIT START
    
    /// MYPLUGIN_FFT_INIT END

}

MyPlugin::~MyPlugin()
{
}

void MyPlugin::render(
    juce::AudioBuffer<float>& buffer,
    int oversamplingFactor)
{
    /// MYPLUGIN_RENDER_BUFFER START

    /// MYPLUGIN_RENDER_BUFFER END
}


void MyPlugin::render(
    juce::dsp::AudioBlock<float>& buffer,
    int oversamplingFactor)
{
    /// MYPLUGIN_RENDER_BLOCK START

    /// MYPLUGIN_RENDER_BLOCK END
}

void MyPlugin::prepare(
    double sampleRate,
    int samplesPerBlock,
    int numChannels)
{
    /// MYPLUGIN_PREPARE START
    /// MYPLUGIN_PREPARE END
}
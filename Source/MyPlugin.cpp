// La classe per gestire tutti i dati del vibrato e per modificare la forma d'onda
#include "MyPlugin.h"

// Chiamato dalla PrepareToPlay. Tieni presente che sampleRate e samplesPerBlock possono cambiare ad ogni pacco dati
MyPlugin::MyPlugin(JX11AudioProcessor *ap)
    : processor(ap)
{
    jassert(processor->value_info_sampleRate >0 && processor->value_info_max_samplesPerBlock > 0);

    //Qui inizializza il plugin presente in Synth.cpp/Synth.h
    realPlugin = std::make_unique<RealPlugin>(processor);
}

MyPlugin::~MyPlugin()
{
}

void MyPlugin::render(juce::AudioBuffer<float> &buffer)
{
    //E qui chiama il codice del plugin (in Synth.h/cpp)
    realPlugin->render(buffer);
}

//Resampling when needed
void MyPlugin::render(juce::dsp::AudioBlock<float> &buffer) {
    //E qui chiama il codice del plugin (in Synth.h/cpp)
    realPlugin->render(buffer);
}

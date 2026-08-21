// La classe per gestire tutti i dati del vibrato e per modificare la forma d'onda
#include "MyPlugin.h"

// Chiamato dalla PrepareToPlay. Tieni presente che sampleRate e samplesPerBlock possono cambiare ad ogni pacco dati
MyPlugin::MyPlugin(JX11AudioProcessor *ap)
    : processor(ap)
{
    /// MYPLUGIN_FFT_INIT START
    
    /// MYPLUGIN_FFT_INIT END

}

MyPlugin::~MyPlugin()
{
}

void MyPlugin::processAudio(
    juce::AudioBuffer<float>& buffer,
    int oversamplingFactor)
{
    /// MYPLUGIN_PROCESS_AUDIO_BUFFER START

    /// MYPLUGIN_PROCESS_AUDIO_BUFFER END
}


void MyPlugin::processAudio(
    juce::dsp::AudioBlock<float>& buffer,
    int oversamplingFactor)
{
    /// MYPLUGIN_PROCESS_AUDIO_BLOCK START

    /// MYPLUGIN_PROCESS_AUDIO_BLOCK END

}

void MyPlugin::prepare(
    double sampleRate,
    int samplesPerBlock,
    int numChannels)
{
    /// MYPLUGIN_PREPARE START
    /// MYPLUGIN_PREPARE END
}

void MyPlugin::reset()
{
    /// MYPLUGIN_RESET START
    /// MYPLUGIN_RESET END
}

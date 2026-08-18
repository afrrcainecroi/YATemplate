#pragma once

// Questa classe gestisce i dati del plugin
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Oscillator.h"
#include "Utils.h"


// Operazioni nel dominio della frequenza!
class DoTheFFTJob
{
public:
    void doTheFFTJob(JX11AudioProcessor *processor, int &fftSize, std::vector<float> &reim)
    {
        // const double fs = processor->value_info_sampleRate;
        // const int half = fftSize / 2;
        // const int cutoffBin = juce::jlimit(0, half, (int)std::floor(processor->value_cutoff * fftSize / fs + 0.5));
        // for (int k = cutoffBin + 1; k < half; ++k)
        // {
        //     reim[2 * k] = 0.0f;
        //     reim[2 * k + 1] = 0.0f;
        // }
        // reim[1] = 0.0f; // Nyquist
    }
};

/// SYNTH_H_RP START

class RealPlugin
{
public:
    void prepare(double sampleRate)
    {
    }
    RealPlugin(JX11AudioProcessor *processor) : processor(processor)
    {
        prepare(processor->value_info_sampleRate);
    }

    void ButtonCallback(int num, juce::String name) {
        std::cout << "Clicked button: " << num << ". Name: " << name << std::endl;
    }

    ~RealPlugin() = default;

    void render(juce::AudioBuffer<float> &buffer) {
    }

    //In case of resampling
    void render(juce::dsp::AudioBlock<float> &buffer) {
    }

private:
    JX11AudioProcessor *processor = nullptr;
};

/// SYNTH_H_RP END

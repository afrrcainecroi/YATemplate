/*
  ==============================================================================

    PluginDSP.h
    Created: 21 Aug 2026 6:11:49pm
    Author:  arcieri

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>


class JX11AudioProcessor;
struct HostTransportInfo;


// ============================================================
// AUDIO DSP CONTEXTS
// ============================================================

struct AudioPrepareContext
{
    double sampleRate = 44100.0;

    int maximumBlockSize = 0;
    int numChannels = 0;

    int oversamplingFactor = 1;
};


struct AudioProcessContext
{
    double sampleRate = 44100.0;

    int oversamplingFactor = 1;
};


// ============================================================
// FFT DSP CONTEXTS
// ============================================================

struct FFTPrepareContext
{
    int fftSize = 0;

    double sampleRate = 44100.0;

    int numChannels = 0;
};


struct FFTProcessContext
{
    int fftSize = 0;

    double sampleRate = 44100.0;

    int channel = 0;
};


// ============================================================
// NORMAL / OVERSAMPLED AUDIO DSP
//
// DEVELOPER-OWNED CODE
// ============================================================

class RealPlugin
{
public:

    explicit RealPlugin(
        JX11AudioProcessor* processorToUse)
        : processor(processorToUse)
    {
    }

    void ButtonCallback(
    int num,
    juce::String name)
{
    juce::ignoreUnused(
        num,
        name);

    // ----------------------------------------------------
    // DEVELOPER:
    //
    // Optional button/event callback.
    // ----------------------------------------------------
}

    void prepare(
        const AudioPrepareContext& context)
    {
        juce::ignoreUnused(context);

        // ----------------------------------------------------
        // DEVELOPER:
        //
        // Allocations and DSP initialisation are allowed here.
        //
        // This instance always belongs to ONE specific
        // oversampling factor.
        // ----------------------------------------------------
    }


    void reset()
    {
        // ----------------------------------------------------
        // DEVELOPER:
        //
        // Reset filters, delays, oscillators, envelopes,
        // history buffers, etc.
        // ----------------------------------------------------
    }


    void processAudio(
        juce::dsp::AudioBlock<float>& block,
        const AudioProcessContext& context,
        const HostTransportInfo& transport)
    {
        juce::ignoreUnused(
            block,
            context,
            transport);

        // ----------------------------------------------------
        // DEVELOPER:
        //
        // REAL-TIME AUDIO DSP
        //
        // No allocations.
        // No locks.
        // No blocking operations.
        // ----------------------------------------------------
    }

    int getLatencySamples() const noexcept
    {
        return 0;
    }



private:

    JX11AudioProcessor* processor = nullptr;
};


// ============================================================
// SPECTRAL DSP
//
// DEVELOPER-OWNED CODE
// ============================================================

class FFTProcessor
{
public:

    explicit FFTProcessor(
        JX11AudioProcessor* processorToUse)
        : processor(processorToUse)
    {
    }


    void prepareFFT(
        const FFTPrepareContext& context)
    {
        juce::ignoreUnused(context);

        // ----------------------------------------------------
        // DEVELOPER:
        //
        // Allocate spectral buffers, masks, tables,
        // smoothing state, freeze buffers, etc.
        //
        // This instance always belongs to ONE fftSize.
        // ----------------------------------------------------
    }


    void resetFFT()
    {
        // ----------------------------------------------------
        // DEVELOPER:
        //
        // Reset spectral processing state.
        // ----------------------------------------------------
    }


    void processFFT(
        std::vector<float>& spectrum,
        const FFTProcessContext& context)
    {
        juce::ignoreUnused(
            spectrum,
            context);

        // ----------------------------------------------------
        // DEVELOPER:
        //
        // REAL-TIME SPECTRAL DSP
        //
        // No allocations.
        // No locks.
        // No blocking operations.
        //
        // spectrum contains the JUCE real-FFT representation.
        // ----------------------------------------------------
    }


private:

    JX11AudioProcessor* processor = nullptr;
};

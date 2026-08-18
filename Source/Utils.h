
#pragma once

#include <JuceHeader.h>
#include <type_traits>
#include <juce_core/juce_core.h>

inline void protectYourEars(float *buffer, int sampleCount);

class DrawingUtils
{
private:
    // Per rapportare le dimensioni dello schermo, sempre identiche, alle dimensioni attuali, derivanti da una resize.
    // 1.618 : 1, rapporto aureo, è il rapporto di default.

public:
    double screenRatio;
    double standardScreenWidth;
    double standardScreenHeight;

    double currentScreenWidth; // Inizializzato a 640 per evitare divisioni per zero
    double currentScreenHeight; // Inizializzato a 400 per evitare divisioni per zero

    DrawingUtils();

    void InitScreenSize(float w, float h);
    void InitCurrentScreenSize(float w, float h);

    void ResetScreenSize();

    void SetCurrentScreenSize(juce::Rectangle<int> &bounds);
    double NormalizeX(double x);
    double NormalizeY(double y);
    juce::Rectangle<int> getScaledBounds(double vx, double vy, double vw, double vh);
};



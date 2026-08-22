
/*
  ==============================================================================

    Utils.cpp
    Created: 14 May 2025 12:10:00pm
    Author:  arcieri

  ==============================================================================
*/

#include "Utils.h"

inline void protectYourEars(float *buffer, int sampleCount)
{
    if (buffer == nullptr)
    {
        return;
    }
    bool firstWarning = true;
    for (int i = 0; i < sampleCount; ++i)
    {
        float x = buffer[i];
        bool silence = false;
        if (std::isnan(x))
        {
            DBG("!!! WARNING: nan detected in audio buffer, silencing !!!");
            silence = true;
        }
        else if (std::isinf(x))
        {
            DBG("!!! WARNING: inf detected in audio buffer, silencing !!!");
            silence = true;
        }
        else if (x < -2.0f || x > 2.0f)
        { // screaming feedback
            DBG("!!! WARNING: sample out of range, silencing !!!");
            silence = true;
        }
        else if (x < -1.0f)
        {
            if (firstWarning)
            {
                DBG("!!! WARNING: sample out of range, clamping !!!");
                firstWarning = false;
            }
            buffer[i] = -1.0f;
        }
        else if (x > 1.0f)
        {
            if (firstWarning)
            {
                DBG("!!! WARNING: sample out of range, clamping !!!");
                firstWarning = false;
            }
            buffer[i] = 1.0f;
        }
        if (silence)
        {
            memset(buffer, 0, sampleCount * sizeof(float));
            return;
        }
    }
}

// La classe drawingutils
DrawingUtils::DrawingUtils()
{

    /// SCREENSIZE START

    screenRatio = 1.618033988749895; // Golden ratio
    standardScreenWidth = 1000.0;
    standardScreenHeight = standardScreenWidth / screenRatio;

    /// SCREENSIZE END

    currentScreenWidth = standardScreenWidth;   // Inizializzato a 640 per evitare divisioni per zero
    currentScreenHeight = standardScreenHeight; // Inizializzato a 400 per evitare divisioni per zero
}

void DrawingUtils::InitScreenSize(float w, float h)
{
    standardScreenWidth = w;
    standardScreenHeight = h;
    screenRatio = standardScreenWidth / standardScreenHeight;
    currentScreenWidth = standardScreenWidth;   // Inizializzato a 640 per evitare divisioni per zero
    currentScreenHeight = standardScreenHeight; // Inizializzato a 400 per evitare divisioni per zero
}

void DrawingUtils::InitCurrentScreenSize(float w, float h)
{
    currentScreenWidth = w;
    currentScreenHeight = h;
}

void DrawingUtils::ResetScreenSize()
{
    currentScreenWidth = standardScreenWidth;
    currentScreenHeight = standardScreenHeight;
}

void DrawingUtils::SetCurrentScreenSize(juce::Rectangle<int> &bounds)
{
    auto doubleBounds = bounds.toFloat();
    InitCurrentScreenSize(doubleBounds.getWidth(), doubleBounds.getHeight());
}

double DrawingUtils::NormalizeX(double x)
{
    auto ret = (x / standardScreenWidth) * currentScreenWidth;
    // DBG("NormalizeX: " << x << " gives: " << ret);
    return ret;
}

double DrawingUtils::NormalizeY(double y)
{
    auto ret = (y / standardScreenHeight) * currentScreenHeight;
    // DBG("NormalizeY: " << y << " gives: " << ret);
    return ret;
}
juce::Rectangle<int> DrawingUtils::getScaledBounds(double vx, double vy, double vw, double vh)
{
    return {
        (int)DrawingUtils::NormalizeX(vx),
        (int)DrawingUtils::NormalizeY(vy),
        (int)DrawingUtils::NormalizeX(vw),
        (int)DrawingUtils::NormalizeY(vh)};
}

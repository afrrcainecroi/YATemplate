#pragma once
#include <JuceHeader.h>

class KineticLookAndFeel : public juce::LookAndFeel_V4
{
public:
    enum class PaletteType { Cyan, Plasma, Gold, Matrix, Fire, Ocean, Toxic, Radon, White, Midnight, Sunset, Mint, Vaporwave, Amber, Crimson, Voltage, Ultraviolet, Stealth };

    KineticLookAndFeel();
    void setPalette(PaletteType type);
    void animatePaletteChange(PaletteType newType, int durationMs = 500);
    void setEditorForRepaint(juce::Component* editor) { editorToRepaint = editor; }

    // Nuova funzione helper che accetta valore e tipo
    static juce::String formatMetric(double value, const juce::String& type);


    struct KineticPalette {
        juce::Colour background;
        juce::Colour trackDark;
        juce::Colour neonCore;   
        juce::Colour neonAux;    
        juce::Colour neonWhite;
        juce::Colour outline;
    };

    void drawBackground(juce::Graphics& g, juce::Rectangle<int> area);

    // Componenti
    void drawLinearSlider(juce::Graphics&, int, int, int, int, float, float, float, const juce::Slider::SliderStyle, juce::Slider&) override;
    void drawRotarySlider(juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
    void drawToggleButton(juce::Graphics&, juce::ToggleButton&, bool, bool) override;
    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override;
    void drawButtonText(juce::Graphics&, juce::TextButton&, bool, bool) override;
    void drawComboBox(juce::Graphics&, int, int, bool, int, int, int, int, juce::ComboBox&) override;
    void drawPopupMenuItem(juce::Graphics&, const juce::Rectangle<int>&, bool, bool, bool, bool, bool, const juce::String&, const juce::String&, const juce::Drawable*, const juce::Colour*) override;
    void drawLabel(juce::Graphics&, juce::Label&) override;

    // Nuovi metodi per Meter e Scope
    virtual void drawKineticMeter (juce::Graphics& g, juce::Rectangle<float> bounds, 
                                   float currentLevel, int style, 
                                   const juce::NamedValueSet& properties);

    virtual void drawKineticScope (juce::Graphics& g, juce::Rectangle<float> bounds, 
                                   const std::array<float, 128>& fifo, int writeIdx,
                                   const juce::NamedValueSet& properties);
    
    // Logica Animazione
    void advanceAnimation();
    bool isAnimating() const { return isTransitioning; }
    void updateGlowPhase() { 
        glowPhase += 0.04f; 
        if (glowPhase > juce::MathConstants<float>::twoPi) 
            glowPhase -= juce::MathConstants<float>::twoPi; 
    }
    //void setShapeImages(const std::vector<juce::Image>& images) { shapeImages = images; }
    // NUOVO METODO: Registra un set di immagini con un nome specifico
    void registerImageSet(const juce::String& setName, const std::vector<juce::Image>& images)
    {
        imageSets[setName] = images;
    }

    KineticPalette currentPalette;
    const KineticPalette& getCurrentPalette() const { return currentPalette; }
private:
    float glowPhase = 0.0f;
    juce::Image backgroundCache;
    
    // Contenitore per le icone PNG
    // std::vector<juce::Image> shapeImages;
    std::map<juce::String, std::vector<juce::Image>> imageSets;
    
    // Cache Sistema Rotary
    std::map<juce::String, juce::Image> rotaryCache;
    void clearCache() { rotaryCache.clear(); backgroundCache = juce::Image(); }
    //juce::String getCacheKey(int w, int h, int ticks, bool labels);
    juce::String getCacheKey(juce::Slider& slider);

    // Transizione palette
    KineticPalette targetPalette;
    KineticPalette startPalette;
    float transitionProgress = 1.0f;
    int transitionDuration = 500;
    juce::int64 transitionStartTime = 0;
    bool isTransitioning = false;
    juce::Component* editorToRepaint = nullptr;

    // --- NUOVI MEMBRI PER IL MOUSE WATCHDOG ---
    juce::Point<int> lastMousePos = { -1, -1 };
    juce::int64 lastMouseMoveTime = 0;
    bool isMouseStatic = false;

    KineticPalette getPaletteForType(PaletteType type);
    
    // Helpers Grafici
    void drawBloom(juce::Graphics& g, const juce::Path& path, juce::Colour color, float thickness);
    void drawComponentTitle(juce::Graphics& g, juce::Rectangle<int>& bounds, const juce::String& title, float fontSize);
    void drawWaveformIcon(juce::Graphics& g, juce::Rectangle<float> area, int iconType, juce::Colour color, float strokeThickness);
    void drawLinearTicks(juce::Graphics& g, juce::Slider& slider, juce::Rectangle<float> area, bool isVertical, float fontSize, float tickLen);
    void drawRotaryTicks(juce::Graphics& g, juce::Slider& slider, float centreX, float centreY, float radius, float startAngle, float endAngle, float fontSize);
};


// ==============================================================================
// 1. KineticMeter: Svuotato della grafica, delega totale al LNF
// ==============================================================================
class KineticMeter : public juce::Component
{
public:
    enum class MeterStyle
    {
        Segmented,
        Analog
    };

    juce::NamedValueSet properties; // Contiene scaleType, isSharp, tickMode, ecc.

    void setStyle(MeterStyle newStyle)
    {
        style = newStyle;
        repaint();
    }

    void updateLevel(float newLevel)
    {
        currentLevel = juce::jmax(newLevel, currentLevel - 0.03f);
        repaint();
    }

    void paint(juce::Graphics &g) override
    {
        if (auto *lnf = dynamic_cast<KineticLookAndFeel *>(&getLookAndFeel()))
        {
            lnf->drawKineticMeter(g, getLocalBounds().toFloat(), currentLevel, (int)style, properties);
        }
    }

private:
    float currentLevel = 0.0f;
    MeterStyle style = MeterStyle::Segmented;
};

// ==============================================================================
// 2. KineticScope: Svuotato della grafica, delega totale al LNF
// ==============================================================================
class KineticScope : public juce::Component
{
public:
    juce::NamedValueSet properties; // Contiene gridStyle, glowMultiplier, isSharp, ecc.

    void fetchFromProcessor(const std::array<std::atomic<float>, 128> &procFifo, int procWriteIdx)
    {
        for (int i = 0; i < 128; ++i)
        {
            fifo[i] = procFifo[i].load(std::memory_order_relaxed);
        }
        writeIdx = procWriteIdx;
        repaint();
    }

    void paint(juce::Graphics &g) override
    {
        if (auto *lnf = dynamic_cast<KineticLookAndFeel *>(&getLookAndFeel()))
        {
            lnf->drawKineticScope(g, getLocalBounds().toFloat(), fifo, writeIdx, properties);
        }
    }

private:
    std::array<float, 128> fifo{0.0f};
    int writeIdx = 0;
};
/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_opengl/juce_opengl.h>
#include "PluginProcessor.h"
#include "KineticLookAndFeel.h"
#include "Utils.h"

/*

// ==============================================================================
// 1. KineticMeter: VU Meter Multi-Stile (Segmentati o Analogici con Lancetta)
// ==============================================================================
class KineticMeter : public juce::Component
{
public:
    enum class MeterStyle
    {
        Segmented,
        Analog
    };

    void setStyle(MeterStyle newStyle)
    {
        style = newStyle;
        repaint();
    }

    void updateLevel(float newLevel)
    {
        // Decadimento fluido (smooth falloff) per una reazione realistica dell'ago/led
        currentLevel = juce::jmax(newLevel, currentLevel - 0.03f);
        repaint();
    }

    void paint(juce::Graphics &g) override
    {
        auto *lnf = dynamic_cast<KineticLookAndFeel *>(&getLookAndFeel());
        if (!lnf)
            return;

        auto palette = lnf->getCurrentPalette();
        auto bounds = getLocalBounds().toFloat();

        if (style == MeterStyle::Segmented)
            paintSegmented(g, bounds, palette);
        else
            paintAnalog(g, bounds, palette);
    }

private:
    float currentLevel = 0.0f;
    MeterStyle style = MeterStyle::Segmented;

    // --- Versione 2.0: Look a Segmenti LED Hardware ---
    void paintSegmented(juce::Graphics &g, juce::Rectangle<float> bounds, const KineticLookAndFeel::KineticPalette &palette)
    {
        bool isVertical = bounds.getHeight() > bounds.getWidth();

        // Impostiamo il range del meter (da -40dB a +6dB)
        float minDB = -40.0f;
        float maxDB = 6.0f;
        float currentDB = juce::jlimit(minDB, maxDB, (float)juce::Decibels::gainToDecibels(currentLevel, minDB));

        // Spazio per i LED e spazio per il testo
        juce::Rectangle<float> ledArea = isVertical ? bounds.removeFromLeft(bounds.getWidth() * 0.6f) : bounds.removeFromTop(bounds.getHeight() * 0.6f);
        juce::Rectangle<float> textArea = bounds;

        // 1. Sfondo scuro LED
        g.setColour(palette.background.darker(0.8f));
        g.fillRoundedRectangle(ledArea, 3.0f);

        int numSegments = 20;
        float gap = 1.5f;

        // 2. Disegna i segmenti
        for (int i = 0; i < numSegments; ++i)
        {
            float prop = (float)i / (numSegments - 1); // 0.0 (basso) -> 1.0 (alto)
            float segDB = juce::jmap(prop, minDB, maxDB);
            bool isLit = (currentDB >= segDB);

            juce::Colour segCol = palette.neonAux.interpolatedWith(palette.neonCore, prop);
            if (!isLit)
                segCol = palette.background.darker(0.3f); // Spento

            if (isVertical)
            {
                float segH = (ledArea.getHeight() - gap) / numSegments;
                float yPos = ledArea.getBottom() - (i + 1) * segH;
                juce::Rectangle<float> rect(ledArea.getX() + gap, yPos, ledArea.getWidth() - (gap * 2), segH - gap);

                g.setColour(segCol);
                g.fillRect(rect);
                if (isLit)
                {
                    g.setColour(palette.neonWhite.withAlpha(0.8f));
                    g.fillRect(rect.reduced(1.0f));
                    g.setColour(segCol.withAlpha(0.4f));
                    g.fillRoundedRectangle(rect.expanded(1.0f), 1.0f); // Glow
                }
            }
        }

        // 3. Etichette Testuali e Graduazioni
        g.setColour(palette.outline.withAlpha(0.8f));
        g.setFont(juce::FontOptions(isVertical ? textArea.getWidth() * 0.45f : textArea.getHeight() * 0.5f));

        std::vector<int> dbMarks = {-40, -24, -12, -6, 0, 3, 6};
        for (int db : dbMarks)
        {
            float prop = juce::jmap((float)db, minDB, maxDB, 0.0f, 1.0f);

            if (isVertical)
            {
                float yPos = ledArea.getBottom() - (prop * ledArea.getHeight());
                g.drawHorizontalLine((int)yPos, ledArea.getRight(), ledArea.getRight() + 3.0f); // Tacca
                g.setColour(db > 0 ? palette.neonCore : palette.neonWhite.withAlpha(0.7f));
                g.drawText(juce::String(db), textArea.withY(yPos - 10).withHeight(20), juce::Justification::centredLeft, false);
            }
        }

        g.setColour(palette.outline.withAlpha(0.6f));
        g.drawRoundedRectangle(ledArea, 3.0f, 1.0f);
    }
    // --- Versione 2.0: Analogico Realistico con Scala Completa ---
    void paintAnalog(juce::Graphics &g, juce::Rectangle<float> bounds, const KineticLookAndFeel::KineticPalette &palette)
    {
        g.setColour(palette.trackDark.darker(0.3f));
        g.fillRoundedRectangle(bounds, 6.0f);
        g.setColour(palette.outline.withAlpha(0.8f));
        g.drawRoundedRectangle(bounds, 6.0f, 1.5f);

        float pivotX = bounds.getCentreX();
        float pivotY = bounds.getBottom() - (bounds.getHeight() * 0.15f);
        float radius = bounds.getHeight() * 0.75f;

        float startAngle = -juce::MathConstants<float>::pi * 0.38f;
        float endAngle = juce::MathConstants<float>::pi * 0.38f;

        float minDB = -40.0f;
        float maxDB = 6.0f;
        float currentDB = juce::jlimit(minDB, maxDB, (float)juce::Decibels::gainToDecibels(currentLevel, minDB));
        float needleAngle = juce::jmap(currentDB, minDB, maxDB, startAngle, endAngle);

        // 1. Etichette dB e Graduazioni
        std::vector<int> dbMarks = {-40, -30, -20, -10, -5, 0, 3, 6};
        g.setFont(juce::FontOptions(radius * 0.15f));

        for (int db : dbMarks)
        {
            float angle = juce::jmap((float)db, minDB, maxDB, startAngle, endAngle);
            bool isMajor = (db == 0 || db % 10 == 0);
            float tickLen = radius * (isMajor ? 0.08f : 0.04f);

            juce::Point<float> outer(pivotX + std::sin(angle) * radius, pivotY - std::cos(angle) * radius);
            juce::Point<float> inner(pivotX + std::sin(angle) * (radius - tickLen), pivotY - std::cos(angle) * (radius - tickLen));

            // Colora di rosso sopra lo zero
            g.setColour(db > 0 ? palette.neonCore : palette.outline.withAlpha(0.6f));
            g.drawLine(inner.x, inner.y, outer.x, outer.y, isMajor ? 1.5f : 1.0f);

            // Testo (spostato leggermente più fuori del raggio)
            if (isMajor || db == 0 || db == 3 || db == 6)
            {
                float textRadius = radius * 1.15f;
                juce::Point<float> textPos(pivotX + std::sin(angle) * textRadius, pivotY - std::cos(angle) * textRadius);
                g.setColour(db > 0 ? palette.neonCore : palette.neonWhite.withAlpha(0.8f));
                g.drawText(juce::String(db), (int)textPos.x - 10, (int)textPos.y - 10, 20, 20, juce::Justification::centred, false);
            }
        }

        // 2. Disegna l'ago
        juce::Point<float> needleEnd(pivotX + std::sin(needleAngle) * radius * 0.95f, pivotY - std::cos(needleAngle) * radius * 0.95f);
        g.setColour(palette.neonCore.withAlpha(0.6f));
        g.drawLine(pivotX, pivotY, needleEnd.x, needleEnd.y, 3.0f); // Glow
        g.setColour(palette.neonWhite);
        g.drawLine(pivotX, pivotY, needleEnd.x, needleEnd.y, 1.2f); // Core

        // 3. Vite centrale
        g.setColour(palette.background.darker());
        g.fillEllipse(pivotX - 8.0f, pivotY - 8.0f, 16.0f, 16.0f);
        g.setColour(palette.outline);
        g.drawEllipse(pivotX - 8.0f, pivotY - 8.0f, 16.0f, 16.0f, 1.5f);
    }
};

// ==============================================================================
// 2. KineticScope: Oscilloscopio in stile Monitor CRT / Fosfori Neon
// ==============================================================================
class KineticScope : public juce::Component
{
public:
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
        auto *lnf = dynamic_cast<KineticLookAndFeel *>(&getLookAndFeel());
        if (!lnf)
            return;

        auto palette = lnf->getCurrentPalette();
        auto bounds = getLocalBounds().toFloat();

        // --- Versione 2.0: Look Monitor CRT retroilluminato ---

        // 1. Sfondo profondo incassato
        g.setColour(palette.background.darker(0.9f));
        g.fillRoundedRectangle(bounds, 6.0f);

        // 2. Griglia (Reticolo) in stile Radar/CRT
        g.setColour(palette.outline.withAlpha(0.12f));
        int numLinesX = 10;
        int numLinesY = 8;
        for (int i = 1; i < numLinesX; ++i)
        {
            float xPos = bounds.getX() + (bounds.getWidth() * i / numLinesX);
            g.drawVerticalLine((int)xPos, bounds.getY(), bounds.getBottom());
        }
        for (int i = 1; i < numLinesY; ++i)
        {
            float yPos = bounds.getY() + (bounds.getHeight() * i / numLinesY);
            g.drawHorizontalLine((int)yPos, bounds.getX(), bounds.getRight());
        }

        // 3. Linea centrale orizzontale marcata (Zero)
        g.setColour(palette.neonAux.withAlpha(0.25f));
        g.drawHorizontalLine((int)bounds.getCentreY(), bounds.getX(), bounds.getRight());

        // 4. Generazione Percorso Forma d'onda
        juce::Path path;
        float midY = bounds.getCentreY();
        float heightFactor = bounds.getHeight() * 0.45f;
        float width = bounds.getWidth();

        int readIdx = writeIdx; // Usiamo l'indice fornito dal processore
        for (int i = 0; i < 128; ++i)
        {
            float x = bounds.getX() + ((i / 127.0f) * width);
            // Limitiamo l'ampiezza per non uscire dal frame
            float sample = juce::jlimit(-1.1f, 1.1f, fifo[readIdx]);
            float y = midY - (sample * heightFactor);

            if (i == 0)
                path.startNewSubPath(x, y);
            else
                path.lineTo(x, y);

            readIdx = (readIdx + 1) % 128; // Ring buffer
        }

        // 5. Effetto Bloom Cyber-Neon (La Traccia dell'oscilloscopio)
        // Disegniamo lo stesso percorso 3 volte con spessori e trasparenze diverse

        // Layer 1: Glow Esterno (largo e sfuocato)
        g.setColour(palette.neonCore.withAlpha(0.25f));
        g.strokePath(path, juce::PathStrokeType(6.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Layer 2: Glow Intermedio
        g.setColour(palette.neonCore.withAlpha(0.6f));
        g.strokePath(path, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Layer 3: Traccia Solida e Brillantissima (Core bianco/neon)
        g.setColour(palette.neonWhite.withAlpha(0.95f));
        g.strokePath(path, juce::PathStrokeType(1.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // 6. Cornice Monitor incassata
        g.setColour(palette.outline.withAlpha(0.7f));
        g.drawRoundedRectangle(bounds, 6.0f, 2.5f);

        // Testo di ampiezza sull'asse Y
        g.setFont(juce::FontOptions(11.0f));
        g.setColour(palette.neonWhite.withAlpha(0.5f));
        g.drawText("+1.0", bounds.getX() + 4, bounds.getY() + 4, 30, 15, juce::Justification::left, false);
        g.drawText(" 0.0", bounds.getX() + 4, bounds.getCentreY() - 7, 30, 15, juce::Justification::left, false);
        g.drawText("-1.0", bounds.getX() + 4, bounds.getBottom() - 18, 30, 15, juce::Justification::left, false);
    }

private:
    std::array<float, 128> fifo{0.0f};
    int writeIdx = 0;
};

*/

//==============================================================================
/**
 */
class JX11AudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    JX11AudioProcessorEditor(JX11AudioProcessor &);
    ~JX11AudioProcessorEditor() override;

    //==============================================================================
    
    
    void paint(juce::Graphics &) override;
    void resized() override;
    void mouseEnter(const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent& event) override
    {
    ///FOOTER_MOUSE START        
        if (event.eventComponent == &footerLink)
        {
            auto textH = 15;
            auto textW = 100;
            juce::Rectangle<int> activeArea (0, footerLink.getHeight() - textH, textW, textH);
            
            if (activeArea.contains(event.getPosition()))
            {
                juce::URL("https://www.af-audio.com").launchInDefaultBrowser();
            }
        }
    ///FOOTER_MOUSE END
    }

    void drawDebugGrid(juce::Graphics &g);
    void paintOverChildren(juce::Graphics &g) override;

    void resizeEditorFromProcessor(int w, int h);

    KineticLookAndFeel kineticLNF;

    juce::TooltipWindow tooltipWindow{this, 500}; // 500ms di ritardo prima di apparire

    /// DECLARATIONS START

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> verticalLogSlider_0001Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> verticalLogSlider_0002Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> materialToggle_0001Attachment;

    // --- Header ---
    juce::Label lblMainTitle; // <--- IL NUOVO TITOLO
    juce::Label lblPalette;
    juce::ComboBox paletteSelector;

    // --- Slider Verticali ---
    juce::Slider slLinVertNum; // Level
    juce::Slider slLinVertLog; // L-Freq
    juce::Slider slLinVertTxt; // Mode
    juce::Slider slMasterGain; // <--- NUOVO: Master Gain

    // --- Slider Orizzontali ---
    juce::Slider slLinHorizNum; // Pan
    juce::Slider slLinHorizTxt; // Wave

    // --- Rotary ---
    juce::Slider slRotNum; // Drive (Lineetta)
    juce::Slider slRotTxt; // Shape (Icona Animata)
    juce::Slider slLogNum; // Freq (Lineetta)

    // --- Bottoni & Controlli ---
    juce::ToggleButton btnToggle; // Limiter
    juce::ToggleButton btnSwitch; // Power
    juce::TextButton btnTrigger;  // Sync
    juce::ComboBox cmbModes;      // Preset

    // --- Footer ---
    juce::Label footerLink; // <--- ECCOLO!
    juce::Label lblCopyright;         // <--- AGGIUNGI QUESTO

    KineticMeter meterIn;
    KineticMeter meterOut;
    KineticScope scopeVisualizer;

    /// DECLARATIONS END

private:
    void timerCallback()
    {
        // Usiamo la stessa logica ottimizzata che abbiamo scritto per advanceAnimation
        kineticLNF.advanceAnimation();

        // --- CONTROLLO HOVER LINK ---
        // Verifichiamo se il mouse è sopra la label

        /// FOOTER_TIMER START
        if (footerLink.isMouseOver())
        {
            // Otteniamo la posizione del mouse relativa alla label
            auto mousePos = footerLink.getMouseXYRelative();
            
            // Calcoliamo l'area approssimativa occupata dal testo (in basso a sinistra)
            // In base al font 12px e alla stringa "www.af-audio.com"
            auto textW = 100; // larghezza stimata del link
            auto textH = 15;  // altezza stimata del font
            
            juce::Rectangle<int> activeArea (0, footerLink.getHeight() - textH, textW, textH);
            
            if (activeArea.contains(mousePos))
            {
                footerLink.setMouseCursor(juce::MouseCursor::PointingHandCursor);
                footerLink.setColour(juce::Label::textColourId, kineticLNF.currentPalette.neonWhite); // Feedback visivo!
            }
            else
            {
                footerLink.setMouseCursor(juce::MouseCursor::NormalCursor);
                footerLink.setColour(juce::Label::textColourId, kineticLNF.currentPalette.neonWhite.withAlpha(0.6f));
            }
        }
        /// FOOTER_TIMER END


        /// TIMER START

        // Aggiorna Meters
        meterIn.updateLevel(ap.meterInPeak.load());
        meterOut.updateLevel(ap.meterOutPeak.load());

        // Aggiorna Oscilloscopio
        scopeVisualizer.fetchFromProcessor(ap.scopeFifo, ap.scopeWriteIdx.load(std::memory_order_relaxed));

        /// TIMER END

    }

    // Dati del layout parsati dal JSON
    juce::var layoutData;

    // Mappa veloce per accoppiare le stringhe del JSON ai componenti C++
    std::unordered_map<juce::String, juce::Component *> componentMap;

public:
    JX11AudioProcessor &ap;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JX11AudioProcessorEditor)
};

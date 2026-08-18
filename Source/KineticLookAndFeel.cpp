#include "KineticLookAndFeel.h"

// ==============================================================================
// HELPER GLOBALE
// ==============================================================================
juce::String KineticLookAndFeel::formatMetric(double value, const juce::String& type)
{
    // --- TIPO: GAIN (dB) ---
    if (type == "gain")
    {
        // Soglia di silenzio (puoi cambiarla a -60 o -96)
        if (value <= -60.0) return "-inf";
        
        // Opzionale: aggiungi il "+" per valori positivi
        if (value > 0.0) return "+" + juce::String(value, 1);
        
        return juce::String(value, 1);
    }

    // --- TIPO: FREQUENZA (Hz) ---
    if (type == "freq" || type == "hz")
    {
        // Se >= 1000Hz, usa "k" (es. 1.2k)
        if (std::abs(value) >= 1000.0) 
        {
            double kVal = value / 1000.0;
            // Se è intero (es. 1000, 2000), niente decimali ("1k")
            // Se ha decimali (es. 1500), mostra un decimale ("1.5k")
            if (std::abs(kVal - (int)kVal) < 0.05) 
                return juce::String((int)kVal) + "k";
            else 
                return juce::String(kVal, 1) + "k";
        }
        // Sotto i 1000Hz, mostra intero (niente decimali per Hz solitamente)
        return juce::String((int)value);
    }

    // --- TIPO: DEFAULT / PERCENTUALE ---
    // Logica standard (gestione k generica e decimali intelligenti)
    if (std::abs(value) >= 1000.0) {
        double kVal = value / 1000.0;
        return juce::String(kVal, (std::abs(kVal - (int)kVal) < 0.05) ? 0 : 1) + "k";
    }
    
    // Se è intero mostra intero, altrimenti 1 decimale
    return juce::String(value, (std::abs(value - (int)value) < 0.05) ? 0 : 1);
}

float getSafeFontSize(float calculatedSize) { return juce::jmax(11.0f, calculatedSize); }

// ==============================================================================
// SETUP & PALETTE
// ==============================================================================
KineticLookAndFeel::KineticLookAndFeel() { setPalette(PaletteType::Midnight); }

void KineticLookAndFeel::setPalette(PaletteType type)
{
    currentPalette = getPaletteForType(type);
    currentPalette.trackDark = juce::Colour::fromString("ff050505");
    currentPalette.outline = juce::Colour::fromString("ff656565");

    setColour(juce::Label::textColourId, currentPalette.neonWhite);
    setColour(juce::ComboBox::textColourId, currentPalette.neonWhite);
    setColour(juce::ComboBox::arrowColourId, currentPalette.neonCore);
    setColour(juce::ComboBox::backgroundColourId, currentPalette.trackDark);
    setColour(juce::PopupMenu::backgroundColourId, currentPalette.background.brighter(0.15f));
    setColour(juce::PopupMenu::textColourId, currentPalette.neonWhite);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, currentPalette.neonCore.withAlpha(0.7f));
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    setColour(juce::TextButton::textColourOffId, currentPalette.neonCore);
    setColour(juce::TextButton::textColourOnId, currentPalette.neonWhite);
    setColour(juce::ToggleButton::textColourId, currentPalette.neonWhite);
    setColour(juce::Slider::textBoxTextColourId, currentPalette.neonWhite);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::thumbColourId, currentPalette.neonCore);

    clearCache();
}

KineticLookAndFeel::KineticPalette KineticLookAndFeel::getPaletteForType(PaletteType type)
{
    KineticPalette pal;
    pal.background = juce::Colour::fromString("ff101010");
    pal.neonWhite = juce::Colour::fromString("ffffffff");
    
    switch (type) {
        case PaletteType::Gold: pal.neonCore = juce::Colour::fromString("ffffaa00"); pal.neonAux = juce::Colour::fromString("ffff2200"); break;
        case PaletteType::Fire: pal.neonCore = juce::Colour::fromString("ffff3300"); pal.neonAux = juce::Colour::fromString("ff880000"); break;
        case PaletteType::Radon: pal.neonCore = juce::Colour::fromString("ffff0099"); pal.neonAux = juce::Colour::fromString("ffff44cc"); break;
        case PaletteType::Cyan: pal.neonCore = juce::Colour::fromString("ff00ffff"); pal.neonAux = juce::Colour::fromString("ff0055aa"); break;
        case PaletteType::Plasma: pal.neonCore = juce::Colour::fromString("ffaa00ff"); pal.neonAux = juce::Colour::fromString("ff6600cc"); break;
        case PaletteType::Matrix: pal.neonCore = juce::Colour::fromString("ff00ff00"); pal.neonAux = juce::Colour::fromString("ff004400"); break;
        case PaletteType::Ocean: pal.neonCore = juce::Colour::fromString("ff0099ff"); pal.neonAux = juce::Colour::fromString("ff002266"); break;
        case PaletteType::Toxic: pal.neonCore = juce::Colour::fromString("ffccff00"); pal.neonAux = juce::Colour::fromString("ff446600"); break;
        case PaletteType::White: pal.neonCore = juce::Colour::fromString("ffffffff"); pal.neonAux = juce::Colour::fromString("ff888888"); break;
        case PaletteType::Sunset: pal.neonCore = juce::Colour::fromString("ffff6600"); pal.neonAux = juce::Colour::fromString("ffff0066"); break;
        case PaletteType::Mint: pal.neonCore = juce::Colour::fromString("ff00ffaa"); pal.neonAux = juce::Colour::fromString("ff008866"); break;
        case PaletteType::Vaporwave: pal.neonCore = juce::Colour::fromString("ffff77aa"); pal.neonAux = juce::Colour::fromString("ff00ddff"); break;
        case PaletteType::Amber: pal.neonCore = juce::Colour::fromString("ffffb000"); pal.neonAux = juce::Colour::fromString("ff552200"); break;
        case PaletteType::Crimson: pal.neonCore = juce::Colour::fromString("ffff0000"); pal.neonAux = juce::Colour::fromString("ff440000"); break;
        case PaletteType::Voltage: pal.neonCore = juce::Colour::fromString("ffffee00"); pal.neonAux = juce::Colour::fromString("ff6600aa"); break;
        case PaletteType::Ultraviolet: pal.neonCore = juce::Colour::fromString("ffaa00ff"); pal.neonAux = juce::Colour::fromString("ff220044"); break;
        case PaletteType::Stealth: pal.neonCore = juce::Colour::fromString("ffddeeff"); pal.neonAux = juce::Colour::fromString("ff224422"); break;
        case PaletteType::Midnight: default: pal.neonCore = juce::Colour::fromString("ff4466ff"); pal.neonAux = juce::Colour::fromString("ff001166"); break;
    }
    return pal;
}

void KineticLookAndFeel::animatePaletteChange(PaletteType newType, int durationMs)
{
    startPalette = currentPalette;
    targetPalette = getPaletteForType(newType);
    transitionDuration = durationMs;
    transitionStartTime = juce::Time::getMillisecondCounter();
    transitionProgress = 0.0f;
    isTransitioning = true;
}

static void repaintHoveredRecursive (juce::Component* c)
{
    if (!c || !c->isVisible())
        return;

    if (auto* s = dynamic_cast<juce::Slider*>(c))
        if (s->isMouseOverOrDragging()) s->repaint();

    if (auto* t = dynamic_cast<juce::ToggleButton*>(c))
        if (t->isMouseOverOrDragging()) t->repaint();

    for (auto* ch : c->getChildren())
        repaintHoveredRecursive(ch);
}


void KineticLookAndFeel::advanceAnimation()
{
    // 1. WATCHDOG DEL MOUSE
    // Recupera la posizione globale del mouse (operazione molto veloce)
    auto currentMousePos = juce::Desktop::getInstance().getMousePosition();
    auto now = juce::Time::getMillisecondCounter();

    if (currentMousePos != lastMousePos)
    {
        // Il mouse si è mosso! Resetta timer e flag.
        lastMousePos = currentMousePos;
        lastMouseMoveTime = now;
        isMouseStatic = false;
    }
    else
    {
        // Il mouse è fermo. Sono passati più di 2.5 secondi?
        if (now > lastMouseMoveTime + 2500)
        {
            isMouseStatic = true;
        }
    }

    // 2. GESTIONE TRANSIZIONE (Ha priorità sul blocco statico)
    if (isTransitioning)
    {
        auto elapsed = now - transitionStartTime;
        transitionProgress = juce::jmin(1.0f, (float)elapsed / (float)transitionDuration);
        float eased = transitionProgress * transitionProgress * (3.0f - 2.0f * transitionProgress);

        currentPalette.neonCore = startPalette.neonCore.interpolatedWith(targetPalette.neonCore, eased);
        currentPalette.neonAux = startPalette.neonAux.interpolatedWith(targetPalette.neonAux, eased);
        clearCache();
        setColour(juce::ComboBox::arrowColourId, currentPalette.neonCore);
        setColour(juce::Slider::thumbColourId, currentPalette.neonCore);

        if (transitionProgress >= 1.0f) isTransitioning = false;
        if (editorToRepaint != nullptr) editorToRepaint->repaint();
        return;
    }

    // 3. BLOCCO RISORSE SE STATICO
    // Se il mouse è fermo da 2.5 secondi e non stiamo cambiando colore,
    // INTERROMPIAMO TUTTO. CPU scende a 0%.
    if (isMouseStatic) return;

    // 4. ANIMAZIONE STANDARD (Solo se il mouse si muove o è appena fermato)
    updateGlowPhase();

    if (editorToRepaint != nullptr)
        repaintHoveredRecursive(editorToRepaint);
}

// juce::String KineticLookAndFeel::getCacheKey(int w, int h, int ticks, bool labels)
// {
//     return juce::String(w) + "_" + juce::String(h) + "_" + juce::String(ticks) + (labels ? "_L" : "_N");
// }

// Sostituisci la vecchia getCacheKey con questa versione robusta
juce::String KineticLookAndFeel::getCacheKey(juce::Slider& slider)
{
    // Creiamo un ID unico basato su dimensioni, stile e range
    // Così Drive (0-10) e Freq (20-20k) avranno cache diverse
    return juce::String(slider.getWidth()) + "_" + 
           juce::String(slider.getHeight()) + "_" + 
           juce::String(slider.getProperties().getWithDefault("tickCount", 0).toString()) + "_" +
           juce::String(slider.getMinimum()) + "_" + 
           juce::String(slider.getMaximum()) + "_" + 
           juce::String(slider.getSkewFactor());
}

void KineticLookAndFeel::drawBackground(juce::Graphics &g, juce::Rectangle<int> area)
{
    if (backgroundCache.isNull() || backgroundCache.getWidth() != area.getWidth() || backgroundCache.getHeight() != area.getHeight())
    {
        backgroundCache = juce::Image(juce::Image::ARGB, area.getWidth(), area.getHeight(), true);
        juce::Graphics g2(backgroundCache);

        g2.fillAll(currentPalette.background);
        g2.setColour(currentPalette.outline.withAlpha(0.08f));
        for (int x = 0; x < area.getWidth(); x += 40) g2.drawVerticalLine(x, 0.0f, (float)area.getHeight());
        for (int y = 0; y < area.getHeight(); y += 40) g2.drawHorizontalLine(y, 0.0f, (float)area.getWidth());

        juce::ColourGradient vignette(juce::Colours::transparentBlack, area.getCentreX(), area.getCentreY(),
                                      juce::Colours::black.withAlpha(0.85f), 0, 0, true);
        g2.setGradientFill(vignette);
        g2.fillAll();

        // if (mainTitle.isNotEmpty())
        // {
        //     auto titleArea = area.removeFromTop(60);
        //     g2.setFont(juce::FontOptions(32.0f).withStyle("Bold"));
        //     g2.setColour(currentPalette.neonWhite);
        //     g2.drawFittedText(mainTitle, titleArea.withTrimmedTop(10), juce::Justification::centred, 1);
        //     g2.setColour(currentPalette.neonCore.withAlpha(0.7f));
        //     g2.fillRect(area.getCentreX() - 50, 55, 100, 4);
        // }
        // auto footer = area.removeFromBottom(30);
        // g2.setColour(juce::Colours::grey);
        // g2.setFont(12.0f);
        // g2.drawText("Copyright (c) 2025 AF-Audio", footer.withTrimmedRight(15), juce::Justification::centredRight, true);
    }
    g.setOpacity(1.0f);
    g.drawImageAt(backgroundCache, 0, 0);
}

void KineticLookAndFeel::drawBloom(juce::Graphics &g, const juce::Path &path, juce::Colour color, float thickness)
{
    // Calcolo pulsazione
    // float sine = (std::sin(glowPhase * 3.0f) + 1.0f) * 0.5f;
    float sine = (std::sin(glowPhase) + 1.0f) * 0.5f;

    // Alpha pulsante tra 0.6 e 1.0 per alta visibilità
    float alpha = 0.6f + (sine * 0.4f);

    // Livello 1: Alone esterno (Ridotto drasticamente a 1.8x)
    g.setColour(color.withAlpha(alpha * 0.3f)); // Più trasparente per sfumare bene
    g.strokePath(path, juce::PathStrokeType(thickness * 2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Livello 2: Core interno (Ridotto a 1.2x - appena sopra la traccia)
    g.setColour(color.withAlpha(alpha * 0.8f)); // Quasi solido
    g.strokePath(path, juce::PathStrokeType(thickness * 1.4f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}


void KineticLookAndFeel::drawComponentTitle(juce::Graphics &g, juce::Rectangle<int> &bounds, const juce::String &title, float fontSize)
{
    if (title.isNotEmpty())
    {
        int height = (int)(fontSize * 1.4f);
        auto titleArea = bounds.removeFromTop(height);
        if (g.getClipBounds().intersects(titleArea)) {
            g.setFont(juce::FontOptions(fontSize).withStyle("Bold"));
            g.setColour(currentPalette.neonWhite);
            g.drawFittedText(title, titleArea, juce::Justification::centred, 1);
        }
        bounds.removeFromTop((int)(height * 0.2f));
    }
}

void KineticLookAndFeel::drawLabel(juce::Graphics &g, juce::Label &label)
{
    if (!label.isBeingEdited()) {
        g.setColour(label.findColour(juce::Label::textColourId));
        
        // Se è la nostra label del copyright, forziamo il font a 12 e togliamo il grassetto
        if (label.getName() == "copyright") 
        {
            g.setFont(juce::FontOptions(12.0f));
        }
        else 
        {
            // Logica originale per tutte le altre label (es. THEME)
            float fontSize = getSafeFontSize(label.getHeight() * 0.45f);
            g.setFont(juce::FontOptions(fontSize).withStyle("Bold"));
        }

        g.drawFittedText(label.getText(), label.getLocalBounds(), label.getJustificationType(), 1);
    }
    // if (!label.isBeingEdited()) {
    //     g.setColour(label.findColour(juce::Label::textColourId));
    //     float fontSize = getSafeFontSize(label.getHeight() * 0.45f);
    //     g.setFont(juce::FontOptions(fontSize).withStyle("Bold"));
    //     g.drawFittedText(label.getText(), label.getLocalBounds(), label.getJustificationType(), 1);
    // }
}

void KineticLookAndFeel::drawRotaryTicks(juce::Graphics &g, juce::Slider &slider, float centreX, float centreY, float radius, float startAngle, float endAngle, float fontSize)
{
    bool showLabels = (bool)slider.getProperties().getWithDefault("showLabels", false);
    juce::String tickMode = slider.getProperties().getWithDefault("tickMode", "all").toString();
    int forcedTicks = (int)slider.getProperties().getWithDefault("tickCount", 0);
    int ticks = (forcedTicks > 1) ? forcedTicks : 11;

    // --- NUOVO: Recupero etichette custom ---
    juce::StringArray customLabels;
    if (slider.getProperties().contains("tickLabels"))
    {
        auto varLabels = slider.getProperties()["tickLabels"];
        if (varLabels.isArray())
        {
            for (auto& v : *varLabels.getArray())
                customLabels.add(v.toString());
        }
    }
    // Se abbiamo etichette custom, forziamo il numero di tick alla dimensione dell'array
    if (!customLabels.isEmpty()) ticks = customLabels.size();
    // ----------------------------------------


    float tickDist = radius * 1.1f;
    float textDist = radius * 1.32f;
    float tickSize = radius * 0.06f;

    // Recupera il tipo
    juce::String type = slider.getProperties().getWithDefault("valueType", "default");

    for (int i = 0; i < ticks; ++i)
    {
        float prop = i / (float)(ticks - 1);

        float angle = startAngle + prop * (endAngle - startAngle);
        //float angle = startAngle + (i / (float)(ticks - 1)) * (endAngle - startAngle);
        float s = std::sin(angle);
        float c = std::cos(angle);

        juce::Line<float> tickLine(centreX + s * (tickDist - tickSize), centreY - c * (tickDist - tickSize),
                                   centreX + s * tickDist, centreY - c * tickDist);
        
        g.setColour(currentPalette.outline.brighter(0.4f));
        g.drawLine(tickLine, 2.0f);

        if (!showLabels) continue;
        
        bool drawLabel = false;
        if (tickMode == "none") drawLabel = false;
        else if (tickMode == "endpoints") drawLabel = (i == 0 || i == ticks - 1);
        else drawLabel = (ticks <= 5) || (i % 2 == 0);

        if (drawLabel)
        {
            //double val = slider.getMinimum() + (i / (float)(ticks - 1)) * (slider.getMaximum() - slider.getMinimum());
            float tx = centreX + s * textDist;
            float ty = centreY - c * textDist;

            juce::String labelText;

            // --- NUOVO: Logica di scelta testo ---
            if (!customLabels.isEmpty() && i < customLabels.size())
            {
                // Caso A: Usa la stringa custom (SINE, SQU, etc.)
                labelText = customLabels[i];
            }
            else
            {
                // Caso B: Calcola il valore numerico
                double val = slider.proportionOfLengthToValue(prop);
                labelText = formatMetric(val, type);
            }
            g.setColour(currentPalette.neonWhite.withAlpha(0.9f));
            g.setFont(juce::FontOptions(fontSize));
            int w = (int)(fontSize * 4.0f); int h = (int)(fontSize * 1.5f);
            g.drawFittedText(labelText, (int)tx - w / 2, (int)ty - h / 2, w, h, juce::Justification::centred, 1);
        }
    }
}

void KineticLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
    // Recupera il tipo
    juce::String type = slider.getProperties().getWithDefault("valueType", "default");
    juce::String suffix = slider.getProperties().getWithDefault("suffix", "").toString();

    juce::Rectangle<int> bounds(x, y, width, height);
    float dim = (float)juce::jmin(width, height);

    float fontSizeTitle = getSafeFontSize(dim * 0.085f);
    float fontSizeVal = getSafeFontSize(dim * 0.075f);
    float fontSizeTicks = getSafeFontSize(dim * 0.065f);

    drawComponentTitle(g, bounds, slider.getProperties()["title"].toString(), fontSizeTitle);

    bool showVal = (bool)slider.getProperties().getWithDefault("showValue", true);
    if (showVal) {
        int valH = (int)(fontSizeVal * 1.5f);
        auto valArea = bounds.removeFromBottom(valH);
        
        juce::String mainText;
        bool customTextFound = false;

        // 1. Controlla se ci sono etichette custom (es. "SIN", "SQU")
        if (slider.getProperties().contains("tickLabels"))
        {
            auto varLabels = slider.getProperties()["tickLabels"];
            if (varLabels.isArray())
            {
                auto* arr = varLabels.getArray();
                // Arrotonda il valore per trovare l'indice (es. 0.0 -> 0, 1.0 -> 1)
                int index = (int)std::round(slider.getValue());
                
                if (index >= 0 && index < arr->size())
                {
                    mainText = (*arr)[index].toString();
                    customTextFound = true;
                }
            }
        }

        // 2. Se non ha trovato etichette custom, usa i numeri standard
        if (!customTextFound)
        {
            juce::String type = slider.getProperties().getWithDefault("valueType", "default");
            juce::String suffix = slider.getProperties().getWithDefault("suffix", "").toString();
            mainText = formatMetric(slider.getValue(), type);
            if (mainText != "-inf") mainText += suffix;
        }

        if (g.getClipBounds().intersects(valArea)) {
            juce::Font f = juce::FontOptions(fontSizeVal);
            // Se è testo (es. "SINE"), lo facciamo leggermente più piccolo se necessario
            if (customTextFound && mainText.length() > 3) f = f.withHeight(fontSizeVal * 0.9f);
            
            g.setColour(currentPalette.neonWhite);
            g.setFont(f);
            // Disegna il testo leggermente più in alto per non uscire dai bordi
            g.drawFittedText(mainText, valArea.translated(0, -(int)(valH * 1.6f)), juce::Justification::centred, 1);
        }
    }
    
    auto trackArea = bounds.toFloat();
    float availableSize = juce::jmin(trackArea.getWidth(), trackArea.getHeight());
    bool showTicks = slider.getProperties().contains("showTicks");
    bool showLabels = (bool)slider.getProperties().getWithDefault("showLabels", false);
    float diameter = availableSize * ((showTicks && showLabels) ? 0.66f : 0.90f);
    float radius = diameter * 0.5f;
    float trackThickness = diameter * 0.08f;
    float centerX = trackArea.getCentreX();
    float centerY = trackArea.getCentreY();

    int ticksCount = (int)slider.getProperties().getWithDefault("tickCount", 0);
    juce::String cacheKey = getCacheKey(slider); //width, height, ticksCount, showLabels);

    if (rotaryCache.find(cacheKey) == rotaryCache.end())
    {
        juce::Image img(juce::Image::ARGB, width, height, true);
        juce::Graphics gCache(img);

        float cX = width * 0.5f;
        float imgCY = (bounds.getY() - y) + bounds.getHeight() * 0.5f;

        if (showTicks)
            drawRotaryTicks(gCache, slider, cX, imgCY, radius, rotaryStartAngle, rotaryEndAngle, fontSizeTicks);

        juce::Path bgArc;
        bgArc.addCentredArc(cX, imgCY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        gCache.setColour(currentPalette.trackDark);
        gCache.strokePath(bgArc, juce::PathStrokeType(trackThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        gCache.setColour(currentPalette.outline.withAlpha(0.5f));
        gCache.strokePath(bgArc, juce::PathStrokeType(1.5f));

        rotaryCache[cacheKey] = img;
    }

    g.drawImageAt(rotaryCache[cacheKey], x, y);

    float toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    if (slider.getValue() > slider.getMinimum())
    {
        juce::Path valArc;
        valArc.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, toAngle, true);
        
        if (slider.isEnabled() && slider.isMouseOverOrDragging())
            drawBloom(g, valArc, currentPalette.neonCore, trackThickness);

        auto arcStart = juce::Point<float>(centerX, centerY).getPointOnCircumference(radius, rotaryStartAngle);
        auto arcEnd = juce::Point<float>(centerX, centerY).getPointOnCircumference(radius, toAngle);
        juce::ColourGradient grad(currentPalette.neonAux, arcStart, currentPalette.neonCore, arcEnd, false);
        g.setGradientFill(grad);
        g.strokePath(valArc, juce::PathStrokeType(trackThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    float dialR = radius - (trackThickness * 1.5f);
    juce::ColourGradient knobGrad(currentPalette.background.brighter(0.15f), centerX, centerY - dialR,
                                  currentPalette.background.darker(0.3f), centerX, centerY + dialR, false);
    g.setGradientFill(knobGrad);
    g.fillEllipse(centerX - dialR, centerY - dialR, dialR * 2, dialR * 2);
    g.setColour(slider.isMouseOverOrDragging() ? currentPalette.neonWhite : currentPalette.outline);
    g.drawEllipse(centerX - dialR, centerY - dialR, dialR * 2, dialR * 2, 2.0f);

    int iconType = -1;
    if (slider.getProperties().contains("morphIcon"))
        iconType = (int)std::round(slider.getValue());
    else
        iconType = (int)slider.getProperties().getWithDefault("iconType", -1);

    if (iconType >= 0)
    {
        // Calcola l'area dove disegnare l'immagine (centro della manopola)
        // dialR è il raggio della parte interna scura
        float imgSize = dialR * 0.85f; // Dimensione immagine (aggiusta il moltiplicatore se serve)
        auto iconArea = juce::Rectangle<float>(0, 0, imgSize, imgSize).withCentre({centerX, centerY});
        float iconStroke = diameter * 0.03f;

        // 1. Recupera il nome del set richiesto dallo slider (es. "shapes")
        juce::String setName = slider.getProperties().getWithDefault("iconSet", "").toString();
        bool imageDrawn = false;

        if (setName.isNotEmpty() && imageSets.find(setName) != imageSets.end())
        {
            // Recupera il vettore di riferimento
            const auto& currentSet = imageSets[setName];

            // 3. Controlla se l'indice è valido per questo set
            if (iconType < currentSet.size() && currentSet[iconType].isValid())            {
                g.setOpacity(1.0f);
                // Disegna l'immagine centrata mantenendo le proporzioni
                g.drawImageWithin(currentSet[iconType],
                    (int)iconArea.getX(), (int)iconArea.getY(), 
                    (int)iconArea.getWidth(), (int)iconArea.getHeight(), 
                    juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize); //stretchToFit
                imageDrawn = true;
            }
        }
        if (!imageDrawn)
        {
            float iconStroke = diameter * 0.03f;
            // Passiamo iconType. Se è fuori range per le forme vettoriali standard (0-3), 
            // drawWaveformIcon dovrebbe gestire il caso o non disegnare nulla.
            drawWaveformIcon(g, iconArea, iconType, currentPalette.neonWhite, iconStroke);
        }

        auto dot = juce::Point<float>(centerX, centerY).getPointOnCircumference(dialR - (iconStroke * 2.0f), toAngle);
        g.setColour(currentPalette.neonCore);
        g.fillEllipse(dot.x - 3, dot.y - 3, 6, 6);

        if (slider.isEnabled() && slider.isMouseOverOrDragging()) {
            float sine = (std::sin(glowPhase) + 1.0f) * 0.5f;
            float glowRad = 8.0f + (sine * 4.0f);
            juce::ColourGradient dotGlow(currentPalette.neonCore.withAlpha(0.9f), dot.x, dot.y,
                                         juce::Colours::transparentBlack, dot.x, dot.y - glowRad, true);
            g.setGradientFill(dotGlow);
            g.fillEllipse(dot.x - glowRad, dot.y - glowRad, glowRad * 2, glowRad * 2);
        }
    } else {
        g.setColour(juce::Colours::white);
        juce::Path p;
        float markerW = diameter * 0.04f; float markerL = dialR * 0.5f;
        p.addRectangle(-markerW / 2, -dialR + (markerL * 0.3f), markerW, markerL);
        p.applyTransform(juce::AffineTransform::rotation(toAngle).translated(centerX, centerY));
        g.fillPath(p);
    }
}

// ==============================================================================
// ALTRI COMPONENTI
// ==============================================================================
void KineticLookAndFeel::drawWaveformIcon(juce::Graphics &g, juce::Rectangle<float> area, int iconType, juce::Colour color, float strokeThickness)
{
    juce::Path p;
    area = area.expanded(area.getWidth() * 0.1f);
    float cx = area.getCentreX(); float cy = area.getCentreY();
    float top = area.getY(); float bottom = area.getBottom();
    float left = area.getX(); float right = area.getRight();
    float w = area.getWidth(); float h = area.getHeight();

    switch (iconType) {
        case 0: p.startNewSubPath(left, cy); p.quadraticTo(left + w * 0.25f, top, cx, cy); p.quadraticTo(left + w * 0.75f, bottom, right, cy); break;
        case 1: p.startNewSubPath(left, cy); p.lineTo(left, top + h * 0.2f); p.lineTo(cx, top + h * 0.2f); p.lineTo(cx, bottom - h * 0.2f); p.lineTo(right, bottom - h * 0.2f); p.lineTo(right, cy); break;
        case 2: p.startNewSubPath(left, bottom); p.lineTo(right, top); p.lineTo(right, bottom); break;
        case 3: p.startNewSubPath(left, cy); p.lineTo(cx, top); p.lineTo(right, cy); break;
    }
    g.setColour(color);
    g.strokePath(p, juce::PathStrokeType(strokeThickness, juce::PathStrokeType::mitered, juce::PathStrokeType::rounded));
}

void KineticLookAndFeel::drawLinearTicks(juce::Graphics &g, juce::Slider &slider, juce::Rectangle<float> area, bool isVertical, float fontSize, float tickLen)
{
    // Recupera il tipo
    juce::String type = slider.getProperties().getWithDefault("valueType", "default");

    if (!g.getClipBounds().intersects(area.toNearestInt())) return;
    bool showTicks = (bool)slider.getProperties().getWithDefault("showTicks", false);
    bool showLabels = (bool)slider.getProperties().getWithDefault("showLabels", false);
    if (!showTicks && !showLabels) return;

    juce::var labelsVar = slider.getProperties()["tickLabels"];
    juce::StringArray customLabels;
    if (labelsVar.isArray()) { auto *a = labelsVar.getArray(); for (auto &v : *a) customLabels.add(v.toString()); }

    int forcedTicks = (int)slider.getProperties().getWithDefault("tickCount", 0);
    int numTicks = customLabels.size();
    if (numTicks == 0) {
        double range = slider.getMaximum() - slider.getMinimum();
        double interval = slider.getInterval();
        numTicks = (forcedTicks > 1) ? forcedTicks : ((interval <= 0 || (range / interval) > 50.0) ? 11 : (int)(range / interval) + 1);
    }
    if (numTicks > 50) numTicks = 11; if (numTicks < 2) return;

    g.setFont(juce::FontOptions(fontSize).withStyle("Bold"));
    float effectiveLen = (isVertical ? area.getHeight() : area.getWidth());
    juce::String tickMode = slider.getProperties().getWithDefault("tickMode", "all").toString();
    float textPadding = 4.0f + (fontSize * 0.2f);
    int skip = (customLabels.isEmpty() && (effectiveLen / numTicks) < (fontSize * 1.2f)) ? 2 : 1;

    for (int i = 0; i < numTicks; ++i) {
        float prop = i / (float)(numTicks - 1);
        bool drawTxt = showLabels;
        if (customLabels.isEmpty()) {
            if (tickMode == "endpoints") drawTxt = (i==0 || i==numTicks-1) && showLabels;
            else if (tickMode == "none") drawTxt = false;
            else if (showLabels) drawTxt = (i % skip == 0);
        }

        if (isVertical) {
            float y = area.getBottom() - prop * effectiveLen;
            if (showTicks) { g.setColour(currentPalette.neonWhite.withAlpha(0.6f)); g.fillRect(area.getX(), y - 1.0f, drawTxt ? tickLen : (tickLen * 0.6f), 2.0f); }
            if (drawTxt) {
                double val = customLabels.isEmpty() ? slider.proportionOfLengthToValue(prop) : 0.0;
                g.setColour(currentPalette.neonWhite);
                float textX = area.getX() + tickLen + textPadding;
                // Passa il tipo!
                juce::String textToDraw = customLabels.isEmpty() ? formatMetric(val, type) : customLabels[i];
                g.drawFittedText(textToDraw,
                                 (int)textX, (int)(y - fontSize * 0.6f), (int)(area.getRight() - textX), (int)(fontSize * 1.2f), juce::Justification::centredLeft, 1);
            }
        } else {
            float x = area.getX() + prop * effectiveLen;
            if (showTicks) { g.setColour(currentPalette.neonWhite.withAlpha(0.6f)); g.fillRect(x - 1.0f, area.getY(), 2.0f, drawTxt ? tickLen : (tickLen * 0.6f)); }
            if (drawTxt) {
                double val = customLabels.isEmpty() ? slider.proportionOfLengthToValue(prop) : 0.0;
                g.setColour(currentPalette.neonWhite);
                float horizontalDist = textPadding * 2.8f;
                // Passa il tipo!
                juce::String textToDraw = customLabels.isEmpty() ? formatMetric(val, type) : customLabels[i];
                g.drawFittedText(textToDraw,
                                 (int)(x - fontSize * 2.0f), (int)(area.getY() + tickLen + horizontalDist), (int)(fontSize * 4.0f), (int)(fontSize * 1.1f), juce::Justification::centred, 1);
            }
        }
    }
}

void KineticLookAndFeel::drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float, float, const juce::Slider::SliderStyle style, juce::Slider &slider)
{
    juce::Rectangle<int> bounds(x, y, width, height);
    bool isVertical = (style == juce::Slider::LinearVertical);
    float scaleBase = isVertical ? (float)width : (float)height;

    float fontSizeTitle = getSafeFontSize(scaleBase * 0.19f);
    float fontSizeVal = getSafeFontSize(scaleBase * 0.17f);
    float fontSizeTick = getSafeFontSize(scaleBase * 0.15f);
    float trackThickness = juce::jmin(scaleBase * 0.15f, 14.0f);
    float thumbW = isVertical ? (scaleBase * 0.40f) : (scaleBase * 0.30f);
    float thumbH = isVertical ? (scaleBase * 0.25f) : (scaleBase * 0.30f);
    float marginX = isVertical ? 0.0f : thumbW * 0.5f;
    float marginY = isVertical ? thumbH * 0.5f : 0.0f;

    drawComponentTitle(g, bounds, slider.getProperties()["title"].toString(), fontSizeTitle);

    if (isVertical && (bool)slider.getProperties().getWithDefault("showValue", true)) {
        int valH = (int)(fontSizeVal * 1.4f);
        auto valArea = bounds.removeFromBottom(valH);
        bounds.removeFromBottom(12);
        if (g.getClipBounds().intersects(valArea)) {
            g.setFont(juce::FontOptions(fontSizeVal));
            g.setColour(currentPalette.neonWhite);
        
            // Recupera il tipo
            juce::String type = slider.getProperties().getWithDefault("valueType", "default");
            juce::String suffix = slider.getProperties().getWithDefault("suffix", "").toString();
            
            juce::String mainText = formatMetric(slider.getValue(), type);
            if (mainText != "-inf") mainText += suffix;

            //juce::String valText = formatMetric(slider.getValue(), type) + suffix;
            g.drawFittedText(mainText,  valArea, juce::Justification::centred, 1);
            //g.drawFittedText(formatValueForDisplay(slider.getValue()) + slider.getProperties().getWithDefault("suffix", "").toString(), valArea, juce::Justification::centred, 1);
        }
    }

    bool showLabels = (bool)slider.getProperties().getWithDefault("showLabels", false);
    juce::Rectangle<int> tickArea;
    if (showLabels || slider.getProperties().contains("showTicks")) {
        float tickSpace = fontSizeTick * 3.5f;
        if (isVertical) {
            int w = juce::jmin(juce::jmax((int)(bounds.getWidth() * 0.5f), 40), bounds.getWidth());
            tickArea = bounds.removeFromRight(w).reduced(0, (int)marginY);
        } else {
            tickArea = bounds.removeFromBottom((int)tickSpace).reduced((int)marginX, 0);
        }
    }

    if (!tickArea.isEmpty()) drawLinearTicks(g, slider, tickArea.toFloat(), isVertical, fontSizeTick, scaleBase * 0.12f);
    if (!isVertical) bounds = bounds.reduced(0, (int)(height * 0.20f));

    auto trackArea = bounds.toFloat();
    juce::Point<float> startPoint, endPoint, thumbPoint;
    if (isVertical) {
        float cx = trackArea.getCentreX();
        startPoint = {cx, trackArea.getBottom() - marginY};
        endPoint = {cx, trackArea.getY() + marginY};
        thumbPoint.x = cx;
        thumbPoint.y = startPoint.y - ((float)slider.valueToProportionOfLength(slider.getValue()) * (startPoint.y - endPoint.y));
    } else {
        float cy = trackArea.getCentreY();
        startPoint = {trackArea.getX() + marginX, cy};
        endPoint = {trackArea.getRight() - marginX, cy};
        thumbPoint.y = cy;
        thumbPoint.x = startPoint.x + ((float)slider.valueToProportionOfLength(slider.getValue()) * (endPoint.x - startPoint.x));
    }

    g.setColour(currentPalette.trackDark);
    juce::Path bgTrack; bgTrack.startNewSubPath(startPoint); bgTrack.lineTo(endPoint);
    g.strokePath(bgTrack, juce::PathStrokeType(trackThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour(currentPalette.outline.withAlpha(0.5f)); g.strokePath(bgTrack, juce::PathStrokeType(1.0f));

    if (slider.getValue() > slider.getMinimum()) {
        juce::ColourGradient grad(currentPalette.neonAux, startPoint.x, startPoint.y, currentPalette.neonCore, thumbPoint.x, thumbPoint.y, false);
        juce::Path fillTrack; fillTrack.startNewSubPath(startPoint); fillTrack.lineTo(thumbPoint);
        if (slider.isEnabled() && slider.isMouseOverOrDragging()) drawBloom(g, fillTrack, currentPalette.neonCore, trackThickness);
        g.setGradientFill(grad); g.strokePath(fillTrack, juce::PathStrokeType(trackThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    juce::Rectangle<float> fader; fader.setSize(thumbW, thumbH); fader.setCentre(thumbPoint);
    if (isVertical) {
        g.setColour(juce::Colours::black.withAlpha(0.5f)); g.fillRoundedRectangle(fader.translated(0, 2), 3.0f);
        juce::ColourGradient bg(currentPalette.background.brighter(0.3f), fader.getX(), fader.getY(), currentPalette.background.darker(0.5f), fader.getRight(), fader.getBottom(), false);
        g.setGradientFill(bg); g.fillRoundedRectangle(fader, 3.0f);
        g.setColour(slider.isMouseOverOrDragging() ? currentPalette.neonWhite : currentPalette.outline); g.drawRoundedRectangle(fader, 3.0f, 1.0f);
        g.setColour(currentPalette.neonCore); g.fillRect(fader.getX() + 3, fader.getCentreY() - 1.0f, fader.getWidth() - 6, 2.0f);
    } else {
        g.setColour(juce::Colours::black.withAlpha(0.5f)); g.fillEllipse(fader.translated(0, 2));
        juce::ColourGradient bg(currentPalette.background.brighter(0.2f), fader.getCentreX(), fader.getCentreY(), currentPalette.background.darker(0.8f), fader.getX(), fader.getY(), true);
        g.setGradientFill(bg); g.fillEllipse(fader);
        g.setColour(slider.isMouseOverOrDragging() ? currentPalette.neonWhite : currentPalette.outline); g.drawEllipse(fader, 1.5f);
        float dotSize = fader.getWidth() * 0.3f;
        auto dotRect = fader.withSizeKeepingCentre(dotSize, dotSize);
        g.setColour(currentPalette.neonCore); g.fillEllipse(dotRect);
        if (slider.isMouseOverOrDragging()) {
            float sine = (std::sin(glowPhase) + 1.0f) * 0.5f; float radius = dotSize * (1.5f + sine * 0.5f);
            juce::ColourGradient glow(currentPalette.neonCore.withAlpha(0.8f), dotRect.getCentreX(), dotRect.getCentreY(), juce::Colours::transparentBlack, dotRect.getCentreX(), dotRect.getCentreY() - radius, true);
            g.setGradientFill(glow); g.fillEllipse(dotRect.withSizeKeepingCentre(radius*2, radius*2));
        }
    }
}

void KineticLookAndFeel::drawToggleButton(juce::Graphics &g, juce::ToggleButton &button, bool, bool)
{
    bool isOn = button.getToggleState();
    auto bounds = button.getLocalBounds().toFloat().reduced(2.0f);
    float h = bounds.getHeight();
    auto textBounds = bounds.removeFromTop(h * 0.45f);
    g.setFont(juce::FontOptions(getSafeFontSize(h * 0.22f)).withStyle("Bold"));
    g.setColour(currentPalette.neonWhite);
    g.drawText(button.getButtonText(), textBounds, juce::Justification::centred, true);

    juce::String style = button.getProperties().getWithDefault("style", "normal").toString();
    if (style == "switch" || button.getProperties().contains("isSwitch")) {
        float swH = h * 0.4f; float swW = swH * 2.2f;
        auto swRect = bounds.withSizeKeepingCentre(swW, swH);
        g.setColour(currentPalette.trackDark); g.fillRoundedRectangle(swRect, swH / 2.0f);
        g.setColour(currentPalette.outline); g.drawRoundedRectangle(swRect, swH / 2.0f, 1.5f);
        float thumbSize = swH * 0.85f;
        float tx = isOn ? swRect.getRight() - thumbSize - 2.0f : swRect.getX() + 2.0f;
        g.setColour(isOn ? currentPalette.neonCore : juce::Colours::grey); g.fillEllipse(tx, swRect.getCentreY() - thumbSize / 2.0f, thumbSize, thumbSize);
        if (isOn && (button.isMouseOver() || button.isDown())) {
            float glowRad = thumbSize * 0.8f;
            juce::ColourGradient glow(currentPalette.neonCore, tx + thumbSize/2, swRect.getCentreY(), juce::Colours::transparentBlack, tx + thumbSize/2, swRect.getCentreY() - glowRad, true);
            g.setGradientFill(glow); g.fillEllipse(tx + thumbSize/2 - glowRad, swRect.getCentreY() - glowRad, glowRad*2, glowRad*2);
        }
    } else {
        float boxSize = h * 0.4f; auto boxRect = bounds.withSizeKeepingCentre(boxSize, boxSize);
        g.setColour(currentPalette.trackDark); g.fillRoundedRectangle(boxRect, 4.0f);
        if (isOn) { g.setColour(currentPalette.neonCore); g.fillRoundedRectangle(boxRect.reduced(2.0f), 2.0f); }
        g.setColour(currentPalette.outline); g.drawRoundedRectangle(boxRect, 4.0f, 1.5f);
    }
}

void KineticLookAndFeel::drawButtonBackground(juce::Graphics &g, juce::Button &button, const juce::Colour &, bool, bool)
{
    auto b = button.getLocalBounds().toFloat().reduced(1);
    g.setColour(currentPalette.background.brighter(0.1f)); g.fillRoundedRectangle(b, 6);
    g.setColour(button.isMouseOver() ? currentPalette.neonCore : currentPalette.outline); g.drawRoundedRectangle(b, 6, 1.5f);
}

void KineticLookAndFeel::drawButtonText (juce::Graphics& g, juce::TextButton& button, bool isMouseOverButton, bool isButtonDown)
{
    // Logica originale per SYNC, POWER, ecc.
    float fontSize = getSafeFontSize(button.getHeight() * 0.32f);
    g.setFont(juce::FontOptions(fontSize).withStyle("Bold"));
    g.setColour(isMouseOverButton ? currentPalette.neonWhite : currentPalette.neonCore);
    g.drawText(button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, true);
}




void KineticLookAndFeel::drawComboBox(juce::Graphics &g, int, int height, bool isButtonDown, int, int, int, int, juce::ComboBox &box)
{
    auto b = box.getLocalBounds().toFloat().reduced(1);
    g.setColour(currentPalette.trackDark); g.fillRoundedRectangle(b, 6);
    g.setColour(box.isMouseOver() ? currentPalette.neonCore : currentPalette.outline);
    if (box.isMouseOver() || isButtonDown) { juce::Path p; p.addRoundedRectangle(b, 6); drawBloom(g, p, currentPalette.neonCore, 1.5f); }
    g.drawRoundedRectangle(b, 6, 1.5f);
    auto arrow = b.removeFromRight(height * 0.6f);
    juce::Path p; float cx = arrow.getCentreX(); float cy = arrow.getCentreY(); float s = height * 0.15f;
    p.addTriangle(cx - s, cy - s / 2, cx + s, cy - s / 2, cx, cy + s);
    g.setColour(currentPalette.neonWhite); g.fillPath(p);
}

void KineticLookAndFeel::drawPopupMenuItem(juce::Graphics &g, const juce::Rectangle<int> &area, bool, bool, bool isHighlighted, bool, bool, const juce::String &text, const juce::String &, const juce::Drawable *, const juce::Colour *)
{
    if (isHighlighted) { g.setColour(currentPalette.neonAux.withAlpha(0.6f)); g.fillRect(area); }
    else { g.setColour(currentPalette.background.brighter(0.15f)); g.fillRect(area); }
    g.setColour(isHighlighted ? currentPalette.neonWhite : currentPalette.neonWhite.withAlpha(0.8f));
    g.setFont(juce::FontOptions(18.0f));
    g.drawText(text, area.reduced(10, 0), juce::Justification::centredLeft, true);
}


void KineticLookAndFeel::drawKineticMeter(juce::Graphics& g, juce::Rectangle<float> bounds, 
                                          float currentLevel, int style, 
                                          const juce::NamedValueSet& properties)
{
    auto palette = getCurrentPalette();
    
    // Estrazione parametri di controllo da properties (con fallback di default)
    bool isSharp = properties.getWithDefault("isSharp", false);
    float glowMultiplier = properties.getWithDefault("glowMultiplier", 1.0f);
    juce::String scaleType = properties.getWithDefault("scaleType", "dB").toString();

    // Sfondo comune
    g.setColour(palette.background.darker(0.8f));
    
    if (style == 0) // Segmented
    {
        bool isVertical = bounds.getHeight() > bounds.getWidth();
        
        // Gestione range di scala dinamico
        float minDB = properties.getWithDefault("rangeMin", -40.0f);
        float maxDB = properties.getWithDefault("rangeMax", 6.0f);
        float currentDB = juce::jlimit(minDB, maxDB, (float)juce::Decibels::gainToDecibels(currentLevel, minDB));

        juce::Rectangle<float> ledArea = isVertical ? bounds.removeFromLeft(bounds.getWidth() * 0.6f) : bounds.removeFromTop(bounds.getHeight() * 0.6f);
        juce::Rectangle<float> textArea = bounds;

        g.fillRoundedRectangle(ledArea, 3.0f);

        int numSegments = properties.getWithDefault("numSegments", 20);
        float gap = 1.5f;

        for (int i = 0; i < numSegments; ++i)
        {
            float prop = (float)i / (numSegments - 1);
            float segDB = juce::jmap(prop, minDB, maxDB);
            bool isLit = (currentDB >= segDB);

            juce::Colour segCol = palette.neonAux.interpolatedWith(palette.neonCore, prop);
            if (!isLit) segCol = palette.background.darker(0.3f);

            if (isVertical)
            {
                float segH = (ledArea.getHeight() - gap) / numSegments;
                float yPos = ledArea.getBottom() - (i + 1) * segH;
                juce::Rectangle<float> rect(ledArea.getX() + gap, yPos, ledArea.getWidth() - (gap * 2), segH - gap);

                g.setColour(segCol);
                g.fillRect(rect);
                
                if (isLit && !isSharp) // Applica contorno sfumato solo se non è Sharp
                {
                    g.setColour(palette.neonWhite.withAlpha(0.8f));
                    g.fillRect(rect.reduced(1.0f));
                    g.setColour(segCol.withAlpha(0.4f * glowMultiplier));
                    g.fillRoundedRectangle(rect.expanded(1.0f), 1.0f);
                }
            }
        }

        // Disegno tacche e label basate su scaleType
        g.setColour(palette.outline.withAlpha(0.8f));
        g.setFont(juce::FontOptions(isVertical ? textArea.getWidth() * 0.45f : textArea.getHeight() * 0.5f));

        std::vector<int> dbMarks;
        if (scaleType == "VU") dbMarks = { -20, -10, -7, -5, -3, -1, 0, 1, 2, 3 };
        else dbMarks = { -40, -24, -12, -6, 0, 3, 6 }; // Default dB

        for (int db : dbMarks)
        {
            float prop = juce::jmap((float)db, minDB, maxDB, 0.0f, 1.0f);
            if (isVertical)
            {
                float yPos = ledArea.getBottom() - (prop * ledArea.getHeight());
                g.drawHorizontalLine((int)yPos, ledArea.getRight(), ledArea.getRight() + 3.0f);
                g.setColour(db > 0 ? palette.neonCore : palette.neonWhite.withAlpha(0.7f));
                
                juce::String labelStr = (scaleType == "linear") ? juce::String(juce::Decibels::decibelsToGain((float)db), 1) : juce::String(db);
                g.drawText(labelStr, textArea.withY(yPos - 10).withHeight(20), juce::Justification::centredLeft, false);
            }
        }
        g.setColour(palette.outline.withAlpha(0.6f));
        g.drawRoundedRectangle(ledArea, 3.0f, 1.0f);
    }
    else // Analog
    {
        g.fillRoundedRectangle(bounds, 6.0f);
        g.setColour(palette.outline.withAlpha(0.8f));
        g.drawRoundedRectangle(bounds, 6.0f, 1.5f);

        float pivotX = bounds.getCentreX();
        float pivotY = bounds.getBottom() - (bounds.getHeight() * 0.15f);
        float radius = bounds.getHeight() * 0.75f;

        float startAngle = -juce::MathConstants<float>::pi * 0.38f;
        float endAngle = juce::MathConstants<float>::pi * 0.38f;

        float minDB = properties.getWithDefault("rangeMin", -40.0f);
        float maxDB = properties.getWithDefault("rangeMax", 6.0f);
        float currentDB = juce::jlimit(minDB, maxDB, (float)juce::Decibels::gainToDecibels(currentLevel, minDB));
        float needleAngle = juce::jmap(currentDB, minDB, maxDB, startAngle, endAngle);

        std::vector<int> dbMarks = { -40, -30, -20, -10, -5, 0, 3, 6 };
        g.setFont(juce::FontOptions(radius * 0.15f));

        for (int db : dbMarks)
        {
            float angle = juce::jmap((float)db, minDB, maxDB, startAngle, endAngle);
            bool isMajor = (db == 0 || db % 10 == 0);
            float tickLen = radius * (isMajor ? 0.08f : 0.04f);

            juce::Point<float> outer(pivotX + std::sin(angle) * radius, pivotY - std::cos(angle) * radius);
            juce::Point<float> inner(pivotX + std::sin(angle) * (radius - tickLen), pivotY - std::cos(angle) * (radius - tickLen));

            g.setColour(db > 0 ? palette.neonCore : palette.outline.withAlpha(0.6f));
            g.drawLine(inner.x, inner.y, outer.x, outer.y, isMajor ? 1.5f : 1.0f);

            float textRadius = radius * 1.15f;
            juce::Point<float> textPos(pivotX + std::sin(angle) * textRadius, pivotY - std::cos(angle) * textRadius);
            g.setColour(db > 0 ? palette.neonCore : palette.neonWhite.withAlpha(0.8f));
            g.drawText(juce::String(db), (int)textPos.x - 10, (int)textPos.y - 10, 20, 20, juce::Justification::centred, false);
        }

        juce::Point<float> needleEnd(pivotX + std::sin(needleAngle) * radius * 0.95f, pivotY - std::cos(needleAngle) * radius * 0.95f);
        
        if (isSharp)
        {
            // Contorno netto per la lancetta
            g.setColour(palette.neonWhite);
            g.drawLine(pivotX, pivotY, needleEnd.x, needleEnd.y, 1.5f);
        }
        else
        {
            // Contorno sfumato (Glow alternato hardware)
            g.setColour(palette.neonCore.withAlpha(0.6f * glowMultiplier));
            g.drawLine(pivotX, pivotY, needleEnd.x, needleEnd.y, 3.0f);
            g.setColour(palette.neonWhite);
            g.drawLine(pivotX, pivotY, needleEnd.x, needleEnd.y, 1.2f);
        }

        g.setColour(palette.background.darker());
        g.fillEllipse(pivotX - 8.0f, pivotY - 8.0f, 16.0f, 16.0f);
        g.setColour(palette.outline);
        g.drawEllipse(pivotX - 8.0f, pivotY - 8.0f, 16.0f, 16.0f, 1.5f);
    }
}

void KineticLookAndFeel::drawKineticScope(juce::Graphics& g, juce::Rectangle<float> bounds, 
                                          const std::array<float, 128>& fifo, int writeIdx, 
                                          const juce::NamedValueSet& properties)
{
    auto palette = getCurrentPalette();
    
    bool isSharp = properties.getWithDefault("isSharp", false);
    float glowMultiplier = properties.getWithDefault("glowMultiplier", 1.0f);
    juce::String gridStyle = properties.getWithDefault("gridStyle", "radar").toString();

    // Sfondo oscilloscopio
    g.setColour(palette.background.darker(0.9f));
    g.fillRoundedRectangle(bounds, 6.0f);

    // Configurazione dinamica della griglia
    if (gridStyle == "radar")
    {
        g.setColour(palette.outline.withAlpha(0.12f));
        int numLinesX = 10;
        int numLinesY = 8;
        for (int i = 1; i < numLinesX; ++i) {
            float xPos = bounds.getX() + (bounds.getWidth() * i / numLinesX);
            g.drawVerticalLine((int)xPos, bounds.getY(), bounds.getBottom());
        }
        for (int i = 1; i < numLinesY; ++i) {
            float yPos = bounds.getY() + (bounds.getHeight() * i / numLinesY);
            g.drawHorizontalLine((int)yPos, bounds.getX(), bounds.getRight());
        }
    }
    else if (gridStyle == "minimal")
    {
        // Solo bordi esterni e zero-axis tratteggiata
        g.setColour(palette.outline.withAlpha(0.08f));
        g.drawHorizontalLine((int)bounds.getCentreY(), bounds.getX(), bounds.getRight());
    }

    // Asse Zero marcato
    g.setColour(palette.neonAux.withAlpha(0.25f));
    g.drawHorizontalLine((int)bounds.getCentreY(), bounds.getX(), bounds.getRight());

    // Ricostruzione percorso traccia
    juce::Path path;
    float midY = bounds.getCentreY();
    float heightFactor = bounds.getHeight() * 0.45f;
    float width = bounds.getWidth();

    int readIdx = writeIdx;
    for (int i = 0; i < 128; ++i)
    {
        float x = bounds.getX() + ((i / 127.0f) * width);
        float sample = juce::jlimit(-1.1f, 1.1f, fifo[readIdx]);
        float y = midY - (sample * heightFactor);

        if (i == 0) path.startNewSubPath(x, y);
        else path.lineTo(x, y);

        readIdx = (readIdx + 1) % 128;
    }

    // Rendering della traccia in base allo sharpness richiesto
    if (isSharp)
    {
        g.setColour(palette.neonWhite);
        g.strokePath(path, juce::PathStrokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    else
    {
        // Spessore e bloom regolati da glowMultiplier
        g.setColour(palette.neonCore.withAlpha(0.25f * glowMultiplier));
        g.strokePath(path, juce::PathStrokeType(6.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        g.setColour(palette.neonCore.withAlpha(0.6f * glowMultiplier));
        g.strokePath(path, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        g.setColour(palette.neonWhite.withAlpha(0.95f));
        g.strokePath(path, juce::PathStrokeType(1.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Cornice esterna
    g.setColour(palette.outline.withAlpha(0.7f));
    g.drawRoundedRectangle(bounds, 6.0f, 2.5f);

    // Assi ampiezza testo
    g.setFont(juce::FontOptions(11.0f));
    g.setColour(palette.neonWhite.withAlpha(0.5f));
    g.drawText("+1.0", bounds.getX() + 4, bounds.getY() + 4, 30, 15, juce::Justification::left, false);
    g.drawText(" 0.0", bounds.getX() + 4, bounds.getCentreY() - 7, 30, 15, juce::Justification::left, false);
    g.drawText("-1.0", bounds.getX() + 4, bounds.getBottom() - 18, 30, 15, juce::Justification::left, false);
}

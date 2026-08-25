#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "MyPlugin.h"
#include <vector>
#include <string>
#include <memory>
#include "Utils.h"
#include <juce_gui_basics/juce_gui_basics.h>

using namespace std;


//==============================================================================
JX11AudioProcessorEditor::JX11AudioProcessorEditor(JX11AudioProcessor &p)
    : AudioProcessorEditor(&p), ap(p)
{
    juce::LookAndFeel::setDefaultLookAndFeel(&kineticLNF);
    kineticLNF.setEditorForRepaint(this);
    startTimer(40);

    /// IMAGE_RESOURCES START

    // std::vector<juce::Image> shapeIcons;

    // // Esempio: Carica da BinaryData (i nomi dipendono da come li hai salvati nel Projucer)
    // shapeIcons.push_back(juce::ImageCache::getFromMemory(BinaryData::wave_sine_png, BinaryData::wave_sine_pngSize));
    // shapeIcons.push_back(juce::ImageCache::getFromMemory(BinaryData::wave_square_png, BinaryData::wave_square_pngSize));
    // shapeIcons.push_back(juce::ImageCache::getFromMemory(BinaryData::wave_ramp_png, BinaryData::wave_ramp_pngSize));
    // shapeIcons.push_back(juce::ImageCache::getFromMemory(BinaryData::wave_triangle_png, BinaryData::wave_triangle_pngSize));
    // shapeIcons.push_back(juce::ImageCache::getFromMemory(BinaryData::wave_iramp_png, BinaryData::wave_iramp_pngSize));

    // // Passa le icone al LookAndFeel
    // kineticLNF.registerImageSet("waves", shapeIcons);

    /// IMAGE_RESOURCES END


    /* Alcune info sullo schermo*/
    juce::Rectangle<int> screenSize = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->userArea;
    DBG("Screen size: " << screenSize.toString());

    /***********************************************************/
    // Inizializzazioni della grid!

    /// GRID START

    //  1. Definiamo il dizionario che collega gli ID del JSON ai componenti reali
    componentMap = {
        {"lblPalette", &lblPalette},
        {"paletteSelector", &paletteSelector},
        {"meterIn", &meterIn},
        {"meterOut", &meterOut},
        {"scopeVisualizer", &scopeVisualizer},
        {"slLinVertNum", &slLinVertNum},
        {"slLinVertLog", &slLinVertLog},
        {"slLinVertTxt", &slLinVertTxt},
        {"slMasterGain", &slMasterGain},
        {"slRotNum", &slRotNum},
        {"slLogNum", &slLogNum},
        {"slRotTxt", &slRotTxt},
        {"slLinHorizNum", &slLinHorizNum},
        {"slLinHorizTxt", &slLinHorizTxt},
        {"btnSwitch", &btnSwitch},
        {"btnToggle", &btnToggle},
        {"btnTrigger", &btnTrigger},
        {"cmbModes", &cmbModes},
        {"footerLink", &footerLink},
        {"lblCopyright", &lblCopyright},
        {"lblMainTitle", &lblMainTitle}
    };

    // 2. Il JSON generato (potrà essere caricato da file in futuro)
    // Griglia 24x12 studiata per dare spazio a Oscilloscopio e Meters
    // juce::String jsonString = R"(
    // {
    //   "grid": { "rows": 15, "columns": 24 },
    //   "components": [
    //     { "var": "lblMainTitle",    "row":  1, "col":  8, "rowSpan":  1, "colSpan":  8 },
    //     { "var": "lblPalette",      "row":  1, "col": 20, "rowSpan":  1, "colSpan":  2, "margin_tb": 12 },
    //     { "var": "paletteSelector", "row":  1, "col": 22, "rowSpan":  1, "colSpan":  3, "margin_tb": 10, "margin_lr": 4 },
        
    //     { "var": "meterIn",         "row":  2, "col":  1,  "rowSpan": 12, "colSpan":  1, "margin_lr": 12 },
        
    //     { "var": "slLinVertNum",    "row":  2, "col":  2,  "rowSpan": 13, "colSpan":  2 },
    //     { "var": "slLinVertLog",    "row":  2, "col":  5,  "rowSpan": 13, "colSpan":  2 },
    //     { "var": "slLinVertTxt",    "row":  2, "col":  8,  "rowSpan": 5, "colSpan":  2 },
        
    //     { "var": "slRotNum",        "row":  2, "col": 11, "rowSpan":  4, "colSpan":  4 },
    //     { "var": "slLogNum",        "row":  2, "col": 15, "rowSpan":  4, "colSpan":  4 },
    //     { "var": "slRotTxt",        "row":  2, "col": 19, "rowSpan":  4, "colSpan":  4 },
        
    //     { "var": "scopeVisualizer", "row":  6, "col": 7, "rowSpan":  3, "colSpan": 10 },
        
    //     { "var": "slLinHorizNum",   "row": 11,"col":   5,  "rowSpan": 2, "colSpan": 18 },
    //     { "var": "slLinHorizTxt",   "row": 14,"col":   5,  "rowSpan": 2, "colSpan": 18 },
        
    //     { "var": "slMasterGain",    "row":  2, "col": 21, "rowSpan":  12, "colSpan":  2 },
    //     { "var": "meterOut",        "row":  2, "col": 24, "rowSpan":  12, "colSpan":  1, "margin_lr": 12 },
        
    //     { "var": "footerLink",      "row": 15,"col":   1,  "rowSpan": 1, "colSpan":  5 },
    //     { "var": "btnSwitch",       "row": 14,"col":   7,  "rowSpan": 1, "colSpan":  2, "margin_tb": 8 },
    //     { "var": "btnToggle",       "row": 14,"col":  10, "rowSpan":  1, "colSpan":  2, "margin_tb": 8 },
    //     { "var": "btnTrigger",      "row": 14,"col":  13, "rowSpan":  1, "colSpan":  2, "margin_tb": 8 },
    //     { "var": "cmbModes",        "row": 14,"col":  16, "rowSpan":  1, "colSpan":  4, "margin_tb": 8 },
    //     { "var": "lblCopyright",    "row": 15,"col":  20, "rowSpan":  1, "colSpan":  4 }
    //   ]
    // }
    // )";

        juce::String jsonString = R"(
    {
      "grid": { "rows": 100, "columns": 100 },
      "components": [
        { "var": "lblMainTitle", "row": 1, "col": 25, "rowSpan": 15, "colSpan": 50 },
        { "var": "meterOut", "row": 10, "col": 99, "rowSpan": 80, "colSpan": 2 },
        { "var": "slMasterGain", "row": 10, "col": 91, "rowSpan": 80, "colSpan": 8 },
        { "var": "paletteSelector", "row": 1, "col": 75, "rowSpan": 8, "colSpan": 16 },
        { "var": "lblPalette", "row": 1, "col": 67, "rowSpan": 8, "colSpan": 8 },
        { "var": "meterIn", "row": 10, "col": 1, "rowSpan": 80, "colSpan": 2 },
        { "var": "slLinVertNum", "row": 10, "col": 3, "rowSpan": 80, "colSpan": 10, "margin-lr": 3 },
        { "var": "footerLink", "row": 1, "col": 1, "rowSpan": 100, "colSpan": 5 }
      ]
    }
    )";









    /// GRID END

    // 3. Esegue il parse una volta sola
    layoutData = juce::JSON::parse(jsonString);




    /***********************************************************/

    // Questo non devi mai eliminarlo!!!!
    /// INTERFACE START

    // HEADER
    meterIn.setStyle(KineticMeter::MeterStyle::Segmented);
    meterIn.properties.set("scaleType", "dB");
    meterIn.properties.set("isSharp", false);       // false = Neon Bloom, true = Hard LED
    meterIn.properties.set("glowMultiplier", 0.5f); // Spinta sull'alone luminoso
    meterIn.properties.set("numSegments", 25);      // Più risoluzione ai led
    addAndMakeVisible(meterIn);

    scopeVisualizer.properties.set("gridStyle", "radar");
    scopeVisualizer.properties.set("isSharp", false);       // false = stile monitor CRT retro
    scopeVisualizer.properties.set("glowMultiplier", 1.5f); // Traccia ad alta luminescenza
    addAndMakeVisible(scopeVisualizer);

    meterOut.setStyle(KineticMeter::MeterStyle::Segmented); // Oppure Segmented, a tua scelta
    meterOut.properties.set("scaleType", "dB");
    meterOut.properties.set("isSharp", false);            // Magari per l'output preferisci un contorno più netto
    meterOut.properties.set("glowMultiplier", 1.5f);     // Bloom standard
    // meterOut.properties.set("rangeMin", -40.0f);         // Range standard per l'output
    // meterOut.properties.set("rangeMax", 6.0f);
    addAndMakeVisible(meterOut);





    lblMainTitle.setText("Cyber Interface", juce::dontSendNotification);
    lblMainTitle.setFont(juce::FontOptions(32.0f).withStyle("Bold"));
    lblMainTitle.setJustificationType(juce::Justification::centredTop);
    lblMainTitle.setColour(juce::Label::textColourId, kineticLNF.currentPalette.neonWhite);
    addAndMakeVisible(lblMainTitle);

    lblPalette.setText("Theme", juce::dontSendNotification);
    lblPalette.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(lblPalette);

    paletteSelector.addItem("Cyan (Cyberpunk)", 1);
    paletteSelector.addItem("Plasma (Purple)", 2);
    paletteSelector.addItem("Gold (Amber)", 3);
    paletteSelector.addItem("Matrix (Green)", 4);
    paletteSelector.addItem("Fire (Red)", 5);
    paletteSelector.addItem("Ocean (Blue)", 6);
    paletteSelector.addItem("Toxic (Lime)", 7);
    paletteSelector.addItem("Radon (Pink)", 8);
    paletteSelector.addItem("White (Mono)", 9);
    paletteSelector.addItem("Midnight (Dark)", 10);
    paletteSelector.addItem("Sunset (Orange)", 11);
    paletteSelector.addItem("Mint (Teal)", 12);
    paletteSelector.addItem("Vaporwave (Pink)", 13);
    paletteSelector.addItem("Amber (Amber)", 14);
    paletteSelector.addItem("Crimson (Red)", 15);
    paletteSelector.addItem("Voltage (Yellow)", 16);
    paletteSelector.addItem("Ultraviolet (Violet)", 17);
    paletteSelector.addItem("Stealth (Grey)", 18);

    paletteSelector.setSelectedId(3);

    // FIX 3: Disabilita cattura tastiera del ComboBox
    paletteSelector.setWantsKeyboardFocus(false);

    paletteSelector.onChange = [this]
    {
        KineticLookAndFeel::PaletteType type;
        switch (paletteSelector.getSelectedId())
        {
        case 1:
            type = KineticLookAndFeel::PaletteType::Cyan;
            break;
        case 2:
            type = KineticLookAndFeel::PaletteType::Plasma;
            break;
        case 3:
            type = KineticLookAndFeel::PaletteType::Gold;
            break;
        case 4:
            type = KineticLookAndFeel::PaletteType::Matrix;
            break;
        case 5:
            type = KineticLookAndFeel::PaletteType::Fire;
            break;
        case 6:
            type = KineticLookAndFeel::PaletteType::Ocean;
            break;
        case 7:
            type = KineticLookAndFeel::PaletteType::Toxic;
            break;
        case 8:
            type = KineticLookAndFeel::PaletteType::Radon;
            break;
        case 9:
            type = KineticLookAndFeel::PaletteType::White;
            break;
        case 10:
            type = KineticLookAndFeel::PaletteType::Midnight;
            break;
        case 11:
            type = KineticLookAndFeel::PaletteType::Sunset;
            break;
        case 12:
            type = KineticLookAndFeel::PaletteType::Mint;
            break;
        case 13:
            type = KineticLookAndFeel::PaletteType::Vaporwave;
            break;
        case 14:
            type = KineticLookAndFeel::PaletteType::Amber;
            break;
        case 15:
            type = KineticLookAndFeel::PaletteType::Crimson;
            break;
        case 16:
            type = KineticLookAndFeel::PaletteType::Voltage;
            break;
        case 17:
            type = KineticLookAndFeel::PaletteType::Ultraviolet;
            break;
        case 18:
            type = KineticLookAndFeel::PaletteType::Stealth;
            break;

        default:
            type = KineticLookAndFeel::PaletteType::Cyan;
            break;
        }
        kineticLNF.animatePaletteChange(type, 2000);
        repaint();
    };
    paletteSelector.setWantsKeyboardFocus(true);
    addAndMakeVisible(paletteSelector);

    // SLIDERS
    // LEVEL (Standard con ticks)
    slLinVertNum.setSliderStyle(juce::Slider::LinearVertical);
    slLinVertNum.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0); // Hide JUCE text
    slLinVertNum.setRange(0.0, 100.0, 0.0);
    slLinVertNum.setValue(80.0);
    slLinVertNum.getProperties().set("title", "LEVEL");
    slLinVertNum.getProperties().set("showTicks", true);
    slLinVertNum.getProperties().set("showValue", true);
    slLinVertNum.getProperties().set("showLabels", true);
    slLinVertNum.getProperties().set("tickMode", "all"); // "all", "endpoints", "none"
    slLinVertNum.setWantsKeyboardFocus(true);
    slLinVertNum.addMouseListener(this, false);
    slLinVertNum.setTooltip("Provami e vedrai...");
    addAndMakeVisible(slLinVertNum);

    // L-FREQ (Logaritmico)
    slLinVertLog.setSliderStyle(juce::Slider::LinearVertical);
    slLinVertLog.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slLinVertLog.setRange(20.0, 20000.0, 0.0);
    slLinVertLog.setSkewFactorFromMidPoint(600.0);
    slLinVertLog.getProperties().set("title", "L-FREQ");
    slLinVertLog.getProperties().set("tickCount", 5);
    slLinVertLog.getProperties().set("showTicks", true);
    slLinVertLog.getProperties().set("showValue", true);
    slLinVertLog.getProperties().set("showLabels", true);
    slLinVertLog.getProperties().set("tickMode", "all");
    slLinVertLog.getProperties().set("suffix", " Hz");
    slLinVertLog.getProperties().set("valueType", "freq");
    slLinVertLog.setWantsKeyboardFocus(true);
    slLinVertLog.addMouseListener(this, false);
    addAndMakeVisible(slLinVertLog);

    // MODE
    slLinVertTxt.setSliderStyle(juce::Slider::LinearVertical);
    slLinVertTxt.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slLinVertTxt.setRange(0.0, 2.0, 1.0);
    slLinVertTxt.getProperties().set("title", "MODE");
    slLinVertTxt.getProperties().set("showTicks", true);
    slLinVertTxt.getProperties().set("showLabels", true);
    juce::StringArray vertLabels = {"LO", "MID", "HI"};
    slLinVertTxt.getProperties().set("tickLabels", juce::var(vertLabels));
    slLinVertTxt.getProperties().set("showValue", true);
    slLinVertTxt.getProperties().set("tickMode", "none");
    slLinVertTxt.setWantsKeyboardFocus(true);
    slLinVertTxt.addMouseListener(this, false);
    addAndMakeVisible(slLinVertTxt);

    // MASTER GAIN
    slMasterGain.setSliderStyle(juce::Slider::LinearVertical);
    slMasterGain.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    // Il range viene controllato dall'Attachment, ma è buona norma settarlo visivamente
    slMasterGain.setRange(-60.0, 12.0, 0.1);
    // Importante: Skew factor per avere lo 0dB verso l'alto (es. a 3/4 della corsa)
    slMasterGain.setSkewFactorFromMidPoint(0.0);

    // Proprietà per KineticLookAndFeel
    slMasterGain.getProperties().set("title", "MASTER");
    slMasterGain.getProperties().set("showTicks", true);
    slMasterGain.getProperties().set("showValue", true);
    slMasterGain.getProperties().set("showLabels", true);
    slMasterGain.getProperties().set("tickMode", "endpoints"); // Mostra solo min/max o custom
    slMasterGain.getProperties().set("valueType", "gain");     // <--- Attiva logica dB e -inf
    slMasterGain.getProperties().set("suffix", "dB");          // Il LNF aggiungerà "dB" al numero
    slMasterGain.getProperties().set("tickMode", "all");

    juce::StringArray masterLabels = {"-inf", "-24", "-12", "-6", "0", "+6", "+12"};
    slMasterGain.getProperties().set("tickLabels", juce::var(masterLabels));
    slMasterGain.getProperties().set("tickMode", "all"); // Mostra tutte quelle dell'array
    // // Custom Labels per la scala in dB (Opzionale ma molto bello visivamente)
    // // KineticLookAndFeel supporta "tickLabels" se passato come var array
    // juce::StringArray gainLabels = { "-inf", "-24", "-12", "0", "+6", "+12" };
    // // Nota: per far funzionare le label custom perfettamente con la scala logaritmica
    // // nel LNF, bisognerebbe mappare le posizioni, ma per ora usiamo la visualizzazione standard
    // // oppure puoi lasciare che il LNF calcoli i numeri.
    // // Se vuoi usare i numeri automatici:
    // slMasterGain.getProperties().set("tickCount", 7);

    slMasterGain.setWantsKeyboardFocus(true);
    addAndMakeVisible(slMasterGain);

    // DRIVE (Rotary con ticks e needle)
    slRotNum.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slRotNum.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slRotNum.setRange(0.0, 10.0, 0.0);
    slRotNum.getProperties().set("title", "DRIVE");
    slRotNum.getProperties().set("showTicks", true); // SCALA GRADUATA INTORNO
    slRotNum.getProperties().set("showLabels", true);
    slRotNum.getProperties().set("showValue", true);
    slRotNum.getProperties().set("tickMode", "all");
    slRotNum.setWantsKeyboardFocus(true);
    slRotNum.addMouseListener(this, false);
    addAndMakeVisible(slRotNum);

    // FREQ
    slLogNum.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slLogNum.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slLogNum.setRange(20.0, 20000.0, 0.0);
    slLogNum.setSkewFactorFromMidPoint(1000.0);
    slLogNum.getProperties().set("title", "FREQ");
    slLogNum.getProperties().set("valueType", "freq");
    slLogNum.getProperties().set("suffix", " Hz");
    slLogNum.getProperties().set("showTicks", true);
    // slLogNum.getProperties().set("tickCount", 7);
    slLogNum.getProperties().set("showLabels", true);
    slLogNum.getProperties().set("tickMode", "all");
    slLogNum.setWantsKeyboardFocus(true);
    slLogNum.addMouseListener(this, false);
    addAndMakeVisible(slLogNum);

    // SHAPE (Icone)
    slRotTxt.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slRotTxt.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slRotTxt.setRange(0.0, 4.0, 1.0);
    slRotTxt.getProperties().set("title", "SHAPE");
    slRotTxt.getProperties().set("tickCount", 4);
    slRotTxt.getProperties().set("showTicks", true);
    slRotTxt.getProperties().set("morphIcon", true);
    // slRotTxt.getProperties().set("tickMode", "none");
    slRotTxt.getProperties().set("showLabels", true);
    slRotTxt.getProperties().set("showValue", true);
    slRotTxt.getProperties().set("iconSet", "waves");
    // FIX 5: Assegna le stringhe da visualizzare SOTTO al rotary
    juce::StringArray shapeLabels = {"SINE", "SQU", "SAW", "TRI", "IRAMP"};
    slRotTxt.getProperties().set("tickLabels", juce::var(shapeLabels));
    slRotTxt.setWantsKeyboardFocus(true);
    slRotTxt.addMouseListener(this, false);
    addAndMakeVisible(slRotTxt);

    // PAN (Orizzontale con tick)
    slLinHorizNum.setSliderStyle(juce::Slider::LinearHorizontal);
    slLinHorizNum.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slLinHorizNum.setRange(-10.0, 10.0, 0.1);
    slLinHorizNum.getProperties().set("title", "PAN");
    slLinHorizNum.getProperties().set("showTicks", true);
    slLinHorizNum.getProperties().set("showValue", true);
    slLinHorizNum.getProperties().set("showLabels", true);
    slLinHorizNum.getProperties().set("tickMode", "all"); // Solo -10 e 10
    slLinHorizNum.setWantsKeyboardFocus(true);
    slLinHorizNum.addMouseListener(this, false);
    addAndMakeVisible(slLinHorizNum);

    // WAVE
    slLinHorizTxt.setSliderStyle(juce::Slider::LinearHorizontal);
    slLinHorizTxt.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slLinHorizTxt.setRange(0.0, 4.0, 1.0);
    slLinHorizTxt.getProperties().set("title", "WAVE");

    slLinHorizTxt.getProperties().set("showTicks", true);
    slLinHorizTxt.getProperties().set("showLabels", true);
    juce::StringArray horizLabels = {"SIN", "SQU", "SAW", "TRI", "IRAMP"};
    slLinHorizTxt.getProperties().set("tickLabels", juce::var(horizLabels));
    slLinHorizTxt.getProperties().set("showValue", false);
    slLinHorizTxt.getProperties().set("tickMode", "all"); // Custom labels = sempre tutte
    slLinHorizTxt.setWantsKeyboardFocus(true);
    slLinHorizTxt.addMouseListener(this, false);
    addAndMakeVisible(slLinHorizTxt);

    // CONTROLS
    btnToggle.setButtonText("LIMITER");
    btnToggle.getProperties().set("isSwitch", true);
    btnToggle.setWantsKeyboardFocus(true); // FIX 3
    btnToggle.addMouseListener(this, false);
    addAndMakeVisible(btnToggle);

    btnSwitch.setButtonText("POWER");
    btnSwitch.getProperties().set("isSwitch", true);
    btnSwitch.setToggleState(true, juce::dontSendNotification);
    btnSwitch.setWantsKeyboardFocus(true); // FIX 3
    btnSwitch.addMouseListener(this, false);
    btnSwitch.onClick = [this]
    { repaint(); };
    addAndMakeVisible(btnSwitch);

    btnTrigger.setButtonText("SYNC");
    btnTrigger.setWantsKeyboardFocus(false); // FIX 3
    btnTrigger.addMouseListener(this, false);
    addAndMakeVisible(btnTrigger);

    cmbModes.addItem("Preset A", 1);
    cmbModes.addItem("Preset B", 2);
    cmbModes.setSelectedId(1);
    cmbModes.setWantsKeyboardFocus(true); // FIX 3
    cmbModes.addMouseListener(this, false);
    addAndMakeVisible(cmbModes);

    // FOOTER
    // --- Footer Link ---
    footerLink.setText("www.af-audio.com", juce::dontSendNotification);
    footerLink.setName("copyright"); // Usiamo lo stesso nome per avere il font 12px dal LNF
    footerLink.setFont(juce::FontOptions(12.0f));
    footerLink.setJustificationType(juce::Justification::bottomLeft);
    footerLink.setColour(juce::Label::textColourId, juce::Colours::grey); //, kineticLNF.currentPalette.neonWhite.withAlpha(0.6f));
    footerLink.setMinimumHorizontalScale(1.0f);
    // Cambia il cursore quando passi sopra (manina)
    footerLink.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    
    // Aggiungiamo il click per aprire l'URL
    footerLink.addMouseListener(this, false); 
    addAndMakeVisible(footerLink);
    
    // footerLink.setButtonText("www.af-audio.com");
    // // Font a 12.0f e allineato in basso a sinistra
    // footerLink.setFont (juce::FontOptions (12.0f), false, juce::Justification::bottomLeft); 
    // footerLink.setURL(juce::URL("https://www.af-audio.com"));
    // footerLink.setColour(juce::HyperlinkButton::textColourId, juce::Colours::white.withAlpha(0.6f));
    // footerLink.setWantsKeyboardFocus(false);
    // addAndMakeVisible(footerLink);

    // --- Copyright Label ---
    lblCopyright.setText("Copyright (c) 2025 AF-Audio", juce::dontSendNotification);
    lblCopyright.setName("copyright"); // <--- AGGIUNGI QUESTO NOME IN CODICE
    lblCopyright.setFont(juce::FontOptions(12.0f)); 
    lblCopyright.setJustificationType(juce::Justification::bottomRight); 
    lblCopyright.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(lblCopyright);
    /// INTERFACE END

    setResizable(true, true);
    DBG("Setting editor size to: " << ap.drawingUtils.currentScreenWidth << "x" << ap.drawingUtils.currentScreenHeight);
    setSize(ap.drawingUtils.currentScreenWidth, ap.drawingUtils.currentScreenHeight);

    setResizeLimits(480, 480 / ap.drawingUtils.screenRatio, 4000, 4000 / ap.drawingUtils.screenRatio);
    getConstrainer()->setFixedAspectRatio(ap.drawingUtils.screenRatio);
    DBG("End screen resizing");

    // FIX: Focus dinamico basato su mouse hover
    setWantsKeyboardFocus(true);
}

JX11AudioProcessorEditor::~JX11AudioProcessorEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    stopTimer();

    /// DESTROY START

    /// DESTROY END
}
void JX11AudioProcessorEditor::resizeEditorFromProcessor(int targetW, int targetH)
{
    // 1. Check di ridondanza
    if (getWidth() == targetW && getHeight() == targetH)
        return;

// 2. Check Wayland (Ancora valido e consigliato)
#if JUCE_LINUX
    if (isShowing())
    {
        DBG("Wayland: Finestra visibile, ignoro il resize per stabilità.");
        repaint();
        return;
    }
#endif

    // 3. Esegui il resize
    if (getConstrainer())
        getConstrainer()->setFixedAspectRatio(0.0);
    setSize(targetW, targetH);
    if (getConstrainer())
        getConstrainer()->setFixedAspectRatio(ap.drawingUtils.screenRatio);
    repaint();
}

void JX11AudioProcessorEditor::mouseEnter(const juce::MouseEvent &event)
{
    // Se il mouse entra in un componente che vuole il focus (come gli slider),
    // glielo diamo immediatamente senza bisogno di cliccare.
    if (auto *component = event.eventComponent)
    {
        if (component->getWantsKeyboardFocus())
            component->grabKeyboardFocus();
    }
}
//==============================================================================
void JX11AudioProcessorEditor::paint(juce::Graphics& g)
{
    kineticLNF.drawBackground(g, getLocalBounds());
}

void JX11AudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    if (area.getWidth() <= 0 || area.getHeight() <= 0) return;
    
    // Calcoliamo quanto la finestra è stata scalata rispetto alla dimensione base
    float scaleX = area.getWidth() / (float)ap.drawingUtils.standardScreenWidth;
    float scaleY = area.getHeight() / (float)ap.drawingUtils.standardScreenHeight;
    float globalScale = std::min(scaleX, scaleY); // Usiamo il minimo per mantenere la leggibilità

    // Applichiamo la scala ai font dei footer per coerenza
    float titleScale = std::min(scaleX, scaleY);
    // Applichiamo la scala al font del titolo (32px di base)

    ///RESIZED START
    lblMainTitle.setFont(juce::FontOptions(32.0f * titleScale).withStyle("Bold"));

    footerLink.setFont(juce::FontOptions(12.0f * globalScale));
    lblCopyright.setFont(juce::FontOptions(12.0f * globalScale));
    ///RESIZED END

    ap.drawingUtils.SetCurrentScreenSize(area);

    if (!layoutData.isObject()) return;

    juce::Grid grid;
    
    int numRows = layoutData["grid"]["rows"];
    int numCols = layoutData["grid"]["cols"];

    for (int i = 0; i < numRows; ++i) 
        grid.templateRows.add(juce::Grid::TrackInfo(juce::Grid::Fr(1)));
    
    for (int i = 0; i < numCols; ++i) 
        grid.templateColumns.add(juce::Grid::TrackInfo(juce::Grid::Fr(1)));

    auto componentsList = layoutData["components"];
    if (componentsList.isArray())
    {
        for (auto& comp : *componentsList.getArray())
        {
            juce::String id = comp["var"].toString();
            
            auto it = componentMap.find(id);
            if (it != componentMap.end())
            {
                juce::Component* target = it->second;
                
                int row = comp["row"];
                int col = comp["col"];
                int rowSpan = comp["rowSpan"];
                int colSpan = comp["colSpan"];

                juce::GridItem item(target);
                item = item.withArea(row, col, row + rowSpan, col + colSpan);
                
                // Calcoliamo i moltiplicatori di scala (basati su standardScreenWidth)
                float scaleX = area.getWidth() / (float)ap.drawingUtils.standardScreenWidth;
                float scaleY = area.getHeight() / (float)ap.drawingUtils.standardScreenHeight;

                // Leggiamo i valori JSON e LI MOLTIPLICHIAMO per la scala
                float m_base = (float)comp.getProperty("margin", 4.0f);
                float m_lr   = (float)comp.getProperty("margin-lr", m_base) * scaleX; // Scala X
                float m_tb   = (float)comp.getProperty("margin-tb", m_base) * scaleY; // Scala Y

                // if (id == "footerLink" || id == "lblCopyright")
                // {
                //     m_tb = 0.0f; // Margine verticale zero
                //     item.alignSelf = juce::GridItem::AlignSelf::end;
                //     item.justifySelf = juce::GridItem::JustifySelf::start; // Per il link a sinistra
                    
                //     // Rimuoviamo eventuali bordi interni della Label che potrebbero rubare pixel
                //     if (auto* lbl = dynamic_cast<juce::Label*>(target))
                //         lbl->setBorderSize(juce::BorderSize<int>(0));
                // }
                
                item.margin = juce::GridItem::Margin(m_tb, m_lr, m_tb, m_lr);
                
                grid.items.add(item);

            }
        }
    }

    grid.performLayout(area.reduced(10));
}

void JX11AudioProcessorEditor::drawDebugGrid(juce::Graphics &g) {
    //Disegna la griglia, se richiesto
    if (layoutData.isObject())
    {
        int numRows = layoutData["grid"]["rows"];
        int numCols = layoutData["grid"]["cols"];
        
        // La griglia è calcolata sull'area ridotta di 10 (come nel resized)
        auto area = getLocalBounds().reduced(10).toFloat();
        
        g.setColour(juce::Colours::cyan.withAlpha(0.35f));
        g.setFont(10.0f);
        
        float cellW = area.getWidth() / numCols;
        float cellH = area.getHeight() / numRows;
        
        // Disegna Righe
        for (int r = 0; r <= numRows; ++r) {
            float y = area.getY() + r * cellH;
            g.drawHorizontalLine((int)y, area.getX(), area.getRight());
            if (r < numRows) g.drawText(juce::String(r + 1), (int)area.getX() + 2, (int)y + 2, 20, 15, juce::Justification::topLeft, false);
        }
        
        // Disegna Colonne
        for (int c = 0; c <= numCols; ++c) {
            float x = area.getX() + c * cellW;
            g.drawVerticalLine((int)x, area.getY(), area.getBottom());
            if (c < numCols) g.drawText(juce::String(c + 1), (int)x + 2, (int)area.getY() + 15, 20, 15, juce::Justification::topLeft, false);
        }
    }
}

void JX11AudioProcessorEditor::paintOverChildren(juce::Graphics &g)
{
    /// PAINT_OVER_CHILDREN START

    // Se il bottone Power (Bypass) è disattivato, o se il parametro AFBypass è true
    // (A seconda di come hai mappato il bottone, supponiamo che false = bypassato)
    if (!bypass.getToggleState())
    {
        // Disegna un velo nero semitrasparente su tutto il plugin
        g.fillAll(juce::Colours::black.withAlpha(0.65f));

        // Opzionale: scrivi "BYPASSED" al centro
        g.setFont(juce::FontOptions(40.0f).withStyle("Bold"));
        g.setColour(kineticLNF.currentPalette.neonWhite.withAlpha(0.8f));
        g.drawFittedText("BYPASSED", getLocalBounds(), juce::Justification::centred, 1);

        // Disabilita tutti i componenti tranne bypass
        for (auto* child : getChildren())
        {
            if (child != &bypass)
            {
                child->setEnabled(false);
            }
        }
    } else {
        //Rimette a posto
        // Abilita tutti i componenti tranne bypass
        for (auto* child : getChildren())
        {
            if (child != &bypass)
            {
                child->setEnabled(true);
            }
        }
    }
    /// PAINT_OVER_CHILDREN END



    // --- GRIGLIA DI DEBUG ---
    // #define DO_GRID 1
    #if DO_GRID
        drawDebugGrid(g);
    #endif
    
}

# YATemplate - Project State

## Purpose

`YATemplate` è il template JUCE usato dal repository `JUCE-Plugin-Generator`.

Il template contiene la struttura C++ di base del plugin e il framework grafico/runtime riutilizzabile.

Il Generator descrive i componenti e genera i blocchi variabili.

YATemplate fornisce:

- struttura `PluginProcessor`;
- struttura `PluginEditor`;
- `KineticLookAndFeel`;
- infrastruttura DSP;
- marker per inserimento codice generato;
- gestione grafica generica;
- risorse statiche.

Il progetto generato deriva dalla combinazione:

```text
Generator + YATemplate -> nuovo progetto JUCE
```

## Repository relationship

Repository correlato:

```text
JUCE-Plugin-Generator
```

Responsabilità:

```text
Generator
    DSL Scheme
    GOOPS classes
    intermediate model
    validation
    C++ emitters
    DSP generation
    APVTS generation
    GUI configuration generation

YATemplate
    JUCE C++ framework
    PluginProcessor
    PluginEditor
    KineticLookAndFeel
    static runtime infrastructure
    generated-code insertion points
```

Il progetto JUCE generato non è la sorgente autorevole.

La sorgente autorevole è la combinazione dei due repository.

## Main source files

```text
Source/KineticLookAndFeel.cpp
Source/KineticLookAndFeel.h

Source/PluginEditor.cpp
Source/PluginEditor.h

Source/PluginProcessor.cpp
Source/PluginProcessor.h

Source/MyPlugin.cpp
Source/MyPlugin.h

Source/Oscillator.h
Source/Synth.h

Source/Utils.cpp
Source/Utils.h
```

Project file:

```text
JX11.jucer
```

Resources:

```text
Resources/wave_iramp.png
Resources/wave_ramp.png
Resources/wave_sine.png
Resources/wave_square.png
Resources/wave_triangle.png
```

`JuceLibraryCode` non fa parte del repository perché è rigenerabile da JUCE/Projucer.

## Generated code markers

I marker canonici usati dal Generator sono:

```cpp
/// INTERFACE START
/// INTERFACE END

/// VALUEPARAMS START
/// VALUEPARAMS END

/// PROCESS START
/// PROCESS END

/// PAINT_OVER_CHILDREN START
/// PAINT_OVER_CHILDREN END
```

I marker emessi non devono contenere `*`.

Esempio errato:

```cpp
///*INTERFACE START
```

La regex usata dal Generator per trovare un marker e la stringa letterale presente nel template sono concetti distinti.

## PluginEditor responsibilities

`PluginEditor` contiene:

- componenti GUI dichiarati dal template;
- blocchi generati dal Generator;
- layout JSON;
- paint / paintOverChildren;
- timer GUI;
- lettura di valori realtime-safe dal processor;
- LookAndFeel;
- palette;
- debug grid.

Il Generator può inserire componenti e configurazioni nei marker definiti.

La logica grafica generica non deve essere duplicata negli emitter Scheme se appartiene naturalmente al template.

## paintOverChildren

Il blocco:

```cpp
/// PAINT_OVER_CHILDREN START

/// PAINT_OVER_CHILDREN END
```

è gestito dal Generator.

Serve per feedback grafici derivati dai role semantici.

### Hard bypass

Role nel Generator:

```text
bypass
```

Comportamento grafico previsto:

- overlay scuro;
- scritta `BYPASSED`;
- altri componenti disabilitati;
- il controllo bypass resta utilizzabile.

Il testo `BYPASSED` deve essere interamente visibile e adattarsi alle dimensioni dell'editor.

### DSP bypass

Role nel Generator:

```text
dsp-bypass
```

Comportamento grafico previsto:

- niente overlay totale;
- GUI ancora utilizzabile;
- feedback riconoscibile separato;
- testo tipo `DSP BYPASSED`.

Hard bypass ha priorità su DSP bypass.

## KineticLookAndFeel

`KineticLookAndFeel` è responsabile della resa grafica generica dei componenti.

Gestisce tra le altre cose:

- palette;
- rotary slider;
- linear slider;
- ticks;
- labels;
- meter;
- scope;
- toggle button;
- switch;
- formattazione dei valori;
- glow;
- colori;
- scaling grafico.

Il Generator non deve duplicare nel C++ generato la logica grafica che appartiene al LookAndFeel.

## Slider properties contract

Il Generator emette properties che YATemplate deve interpretare.

Properties attualmente definite per gli slider:

```text
title
valueType
suffix
showValue
showTicks
showLabels
tickCount
tickMode
tickLabels
```

Queste derivano dalla DSL Scheme:

```text
title
value-type
suffix
show-value
show-ticks
show-labels
tick-count
tick-mode
tick-labels
```

Il contratto è:

```text
DSL property
    |
    v
Generator
    |
    v
JUCE Component::getProperties()
    |
    v
KineticLookAndFeel
```

Le properties non devono essere ignorate o sostituite da logica hardcoded.

## Slider value rendering

`KineticLookAndFeel::formatMetric()` centralizza la formattazione dei valori.

Tipi attualmente gestiti:

```text
gain
freq
hz
default
```

Comportamento corrente:

```text
gain:
-60.0 -> -inf
0.0   -> 0.0
3.5   -> +3.5

freq:
1000  -> 1k
1500  -> 1.5k
13230 -> 13.2k
```

Il `suffix` viene aggiunto separatamente quando necessario.

Esempio:

```text
13.2k + " Hz" -> "13.2k Hz"
0.0   + " dB" -> "0.0 dB"
```

I tick numerici usano anch'essi `formatMetric()`.

## JUCE Slider TextBox

Il valore viene disegnato dal `KineticLookAndFeel`.

Il TextBox standard JUCE non deve essere visibile.

Il Generator deve quindi produrre:

```cpp
slider.setTextBoxStyle(
    juce::Slider::NoTextBox,
    false,
    0,
    0);
```

per rotary e linear slider.

Questo impedisce al TextBox standard di mostrare valori raw come:

```text
13230.100585...
0.000000...
```

La rimozione del TextBox non sostituisce il sistema delle properties.

## Palette system

`KineticLookAndFeel` gestisce più palette.

Il selector GUI deve essere sincronizzato con la palette realmente applicata.

Problema noto:

il selector può mostrare un nome di palette mentre la palette effettivamente attiva è diversa se l'inizializzazione usa `dontSendNotification` senza applicare esplicitamente la palette.

Questo punto è ancora da verificare/completare.

## Meter architecture

I componenti meter sono grafici.

Il processor non deve conoscere classi GUI come `KineticMeter`.

Il processor espone invece valori realtime-safe.

La GUI li legge periodicamente.

I meter semantici sono gestiti dal Generator tramite role:

```text
input-meter
output-meter
```

Il calcolo del peak deve idealmente considerare tutti i canali:

```cpp
float peak = 0.0f;

for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
{
    peak = juce::jmax(
        peak,
        buffer.getMagnitude(
            ch,
            0,
            buffer.getNumSamples()));
}
```

## Scope architecture

Il role semantico nel Generator è:

```text
scope
```

Il processor può alimentare una FIFO o struttura equivalente.

La GUI consuma i dati fuori dal realtime path.

L'implementazione corrente può usare il channel 0 come prima versione.

## PluginProcessor responsibilities

`PluginProcessor` contiene:

- APVTS;
- parametri;
- stato runtime;
- `processBlock`;
- dati realtime-safe per meter/scope;
- infrastruttura DSP;
- istanza `myplugin`.

Il blocco generato:

```cpp
/// VALUEPARAMS START

/// VALUEPARAMS END
```

carica i valori correnti dei parametri.

Il blocco:

```cpp
/// PROCESS START

/// PROCESS END
```

contiene la pipeline DSP generata.

## Current DSP pipeline

La pipeline desiderata è:

```text
HARD BYPASS
    |
INPUT GAIN
    |
INPUT METER
    |
DRY COPY           [future wet/dry]
    |
DSP BYPASS
    |
OVERSAMPLING       [future]
    |
myplugin->render()
    |
DOWNSAMPLING       [future]
    |
WET/DRY MIX        [future]
    |
OUTPUT GAIN
    |
OUTPUT METER
    |
SCOPE
```

### Hard bypass

Quando attivo:

```cpp
return;
```

prima di ogni altra elaborazione.

Il buffer deve uscire invariato.

### DSP bypass

Quando attivo:

```text
myplugin->render()
```

viene saltato.

Input/output gain, meter e scope possono continuare a funzionare.

## Gain semantics

I gain sono specificati in dB.

`AudioBuffer::applyGain()` richiede gain lineare.

Il codice corretto è:

```cpp
buffer.applyGain(
    juce::Decibels::decibelsToGain(value_inputGain));
```

e analogamente per output gain.

## Wet/Dry future integration

Il role previsto nel Generator è:

```text
wet-dry
```

YATemplate dovrà supportare:

- dry buffer;
- preparazione buffer;
- copia realtime-safe;
- wet processing;
- mix finale.

Non devono esserci allocazioni dinamiche in `processBlock()`.

## Oversampling future integration

Il role previsto nel Generator è:

```text
oversampling
```

L'oversampling richiederà supporto template per:

- dichiarazione oggetto;
- prepare;
- reset;
- processSamplesUp;
- DSP processing;
- processSamplesDown.

Non deve essere trattato come semplice frammento casuale di `processBlock`.

## Debug grid

La griglia di debug non è una funzionalità semantica del plugin.

Non esiste un role `grid-onoff`.

`drawDebugGrid()` può restare funzione statica del template e può essere condizionata da:

```cpp
#if JUCE_DEBUG
    drawDebugGrid(g);
#endif
```

o altra define dedicata.

## Layout

Per ora il layout finale è ricevuto già risolto dal Generator.

Il C++ usa:

```text
row
col
rowSpan
colSpan
margin_tb
margin_lr
```

La chiave JSON corrente della griglia è:

```text
cols
```

non:

```text
columns
```

L'algoritmo di layout automatico resterà nel Generator.

YATemplate deve solo applicare il layout finale.

## UUID / VST3 CID

L'identità del plugin deve essere stabile tra rigenerazioni dello stesso progetto.

YATemplate non deve introdurre comportamenti che cambino arbitrariamente UUID/CID.

La gestione della persistenza appartiene al Generator.

## Development rules

Le modifiche devono rispettare questa divisione:

```text
Generator:
    cosa generare
    quando generarlo
    configurazione
    semantic roles
    model

YATemplate:
    runtime implementation
    generic GUI behaviour
    reusable framework
    static JUCE infrastructure
```

Non correggere manualmente un progetto generato se il problema appartiene a Generator o YATemplate.

Workflow:

```text
modify source repository
        |
        v
commit
        |
        v
generate plugin
        |
        v
compile
        |
        v
test
```

## Current state

Attualmente sono già presenti o supportati:

```text
KineticLookAndFeel
palette system
rotary slider
linear slider
meters
scope
toggle/switch
PluginProcessor APVTS framework
PluginEditor framework
PROCESS generated block
VALUEPARAMS generated block
PAINT_OVER_CHILDREN generated block
```

Sono stati verificati:

```text
formatMetric()
valueType property
suffix property
showValue property
tick properties
```

Problemi grafici ancora da completare:

```text
BYPASSED text fitting
DSP BYPASSED visual feedback
meter sizing
palette initial synchronization
```

## Source of truth

Il repository Git corrente è la fonte autorevole.

Vecchie copie del template, backup e progetti generati non devono essere usati come riferimento se divergono da questo repository.

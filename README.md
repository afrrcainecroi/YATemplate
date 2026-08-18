## License

This project is licensed under the MIT License.

Copyright (c) 2025- Franco Arcieri (afrrcainecroi).


# Manuale d'Uso: Proprietà del KineticLookAndFeel

Il `KineticLookAndFeel` personalizzato sfrutta il sistema di proprietà (`NamedValueSet`) di `juce::Component` per modificare dinamicamente il rendering di Slider e Bottoni senza la necessità di creare classi derivate. 

Di seguito è riportato l'elenco completo delle **14 chiavi** supportate dal motore grafico.

## 🎚️ 1. Proprietà Generali per Slider (Rotary e Lineari)
Queste proprietà controllano l'aspetto di base, la titolazione e la formattazione dei valori testuali/numerici.

* **`title`** *(String)*: Il testo mostrato in alto, sopra lo slider o la manopola.
  * *Esempio:* `slider.getProperties().set("title", "L-FREQ");`
* **`showValue`** *(bool)*: Se `true`, mostra il valore numerico (o l'etichetta di testo corrente) nella parte inferiore del componente.
  * *Esempio:* `slider.getProperties().set("showValue", true);`
* **`valueType`** *(String)*: Indica al LookAndFeel come formattare il numero mostrato.
  * `"default"` (o non impostato): Formattazione standard (utilizza la 'k' per valori >= 1000).
  * `"freq"` o `"hz"`: Formatta per le frequenze (es. 1500 diventa "1.5k", omettendo i decimali sotto i 1000Hz).
  * `"gain"`: Formatta per i dB. Aggiunge il segno `+` ai valori positivi e mostra `-inf` per valori <= -60.0.
* **`suffix`** *(String)*: Una stringa accodata automaticamente al valore numerico.
  * *Esempio:* `slider.getProperties().set("suffix", " Hz");`

## 📏 2. Gestione delle Tacche (Ticks) e delle Etichette
Queste proprietà regolano la scala graduata disegnata attorno ai Rotary Slider o di lato ai Linear Slider.

* **`showTicks`** *(bool)*: Disegna i trattini della scala graduata.
* **`showLabels`** *(bool)*: Disegna i testi (numeri o stringhe) in corrispondenza delle tacche.
* **`tickMode`** *(String)*: Decide *quali* etichette disegnare per evitare sovrapposizioni visive.
  * `"all"`: Disegna l'etichetta su ogni tacca (o in modo alternato se sono troppe).
  * `"endpoints"`: Disegna solo i valori agli estremi (minimo e massimo).
  * `"none"`: Non disegna alcuna etichetta sulla scala graduata.
* **`tickCount`** *(int)*: Forza un numero specifico di tacche. Se non impostato (o a 0), il sistema le calcola in automatico oppure utilizza il numero di elementi presenti in `tickLabels`.
* **`tickLabels`** *(juce::var Array)*: Sostituisce i numeri calcolati con testi personalizzati (es. forme d'onda o modalità). Se impostato, fissa automaticamente il `tickCount` alla dimensione dell'array.
  * *Esempio:* ```cpp
    juce::StringArray labels = {"LO", "MID", "HI"};
    slider.getProperties().set("tickLabels", juce::var(labels));
    ```

## 🎛️ 3. Icone Centrali (Solo Rotary Slider)
Il LookAndFeel permette di disegnare icone vettoriali o di visualizzare immagini PNG personalizzate al centro della manopola.

* **`iconSet`** *(String)*: Richiama un set di immagini PNG registrato nel LookAndFeel tramite `kineticLNF.registerImageSet()`. Mostra l'immagine corrispondente al valore attuale arrotondato (es. valore 2 = terza immagine dell'array).
  * *Esempio:* `slider.getProperties().set("iconSet", "waves");`
* **`iconType`** *(int)*: Utilizzato se `iconSet` non è definito. Disegna un'icona vettoriale di base:
  * `0` = Sine
  * `1` = Square
  * `2` = Saw/Ramp
  * `3` = Triangle
* **`morphIcon`** *(bool)*: Se `true` e si stanno utilizzando le forme vettoriali base, l'icona si trasforma ("morpha") fluidamente tra le forme 0-3 in base alla rotazione della manopola (il valore dello slider viene mappato da 0.0 a 3.99).

## 🔘 4. Bottoni (ToggleButtons)
Controllano l'aspetto dei pulsanti a due stati (`juce::ToggleButton`).

* **`isSwitch`** *(bool)*: Modifica radicalmente la renderizzazione del bottone.
  * `false` (o non impostato): Disegna una classica checkbox quadrata in stile cyber.
  * `true`: Disegna un interruttore a scorrimento (switch) orizzontale.
* **`style`** *(String)*: Un'alternativa testuale a `isSwitch`.
  * `"switch"`: Equivalente a `isSwitch = true`.
  * `"normal"` (o non impostato): Equivalente a `isSwitch = false`.

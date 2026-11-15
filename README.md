# PROGETTO: Replica Super Mario Bros per NES
## Compilazione
Rimanendo nella cartella principale, per compilare (vengono compilate tutte le tappe):
```
cmake -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build
```
## Esecuzione
Per eseguire una tappa:
```
./build/{nome_eseguibile_tappa}
```
I nomi degli eseguibili delle varie tappe sono:
```
 - 01_basic_structure
 - 02_character_movement
 - 03_pause_menu
```

## Comandi
Nel menu di pausa:
 - ESC: riprendi a giocare
 - UP (↑): cambia la scelta selezionando quella sopra
 - DOWN (↓): cambia la scelta selezionando quella sotto
 - ENTER: conferma la scelta selezionata

In gioco:
 - ESC: mettere il gioco in pausa
 - LEFT (←): muovere il personaggio a sinistra
 - RIGHT (→): muovere il personaggioa  destra
 - UP (↑): far saltare il personaggio
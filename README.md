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
```

## Comandi
 - ESC: mettere il gioco in pausa
 - LEFT (←): muovere il personaggio a sinistra
 - RIGHT (→): muovere il personaggioa  destra
 - UP (↑): far saltare il personaggio
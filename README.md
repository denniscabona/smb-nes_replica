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
 - 04_power_up_introduction
 - 05_damage_introduction
 - 06_better_ground_and_collisions
 - 07_power_up_and_enemy_improvement
 - 08_struct_system_improvement
 - 09_camera_scrolling
 - 10_enemy_movement
 - 11_power_up_completion
 - 12_enemies_completion
 - 13_level_creation_with_tilemap
```

## Comandi
Nel menu di pausa:
 - ESC: riprendi a giocare
 - UP (↑): cambia la scelta selezionando quella sopra
 - DOWN (↓): cambia la scelta selezionando quella sotto
 - ENTER: conferma la scelta selezionata

In gioco:
 - ESC: mettere il gioco in pausa
 - LEFT (←) oppure A: muovere il personaggio a sinistra
 - RIGHT (→) oppure D: muovere il personaggioa  destra
 - UP (↑) oppure W: far saltare il personaggio
 - E: spara la pallina di fuoco (solo se si ha il power-up del Fiore di Fuoco)
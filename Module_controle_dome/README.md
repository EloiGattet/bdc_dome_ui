# Module de contrôle dôme — Arduino Nano

## Structure du code (2024)

- **main.cpp** : Logique de navigation principale (états, setup/loop, transitions entre menus)
- **menus.cpp / menus.h** : Tableaux de chaînes pour tous les menus (centralisation DRY)
- **acquisition.cpp** : Affichage et logique du sous-menu Acquisition
- **tests.cpp** : Affichage et logique du sous-menu Tests (inclut tous les tests matériels)
- **reglages.cpp** : Affichage et logique du sous-menu Réglages (édition des paramètres)
- **affichage.cpp** : Fonctions d'affichage génériques (drawMenu, drawMenuTitle, splashScreen...)
- **hardware.cpp** : Initialisation et gestion des pins, objets globaux (écran, ring, 7 segments...)
- **logo.cpp** : Bitmap du logo Mercurio

**Centralisation DRY** :
- Tous les tableaux de chaînes de menu sont définis une seule fois dans `menus.cpp` et accessibles partout via `menus.h` (avec `extern`).
- Les sous-menus sont dans des fichiers dédiés, chaque module gère uniquement son affichage et ses actions.

**Navigation** :
- La logique de navigation (états, transitions, gestion des boutons) reste dans `main.cpp`.
- Les sous-menus sont appelés via des fonctions dédiées (ex : `drawTestMenu`, `drawReglageMenu`, etc.).

## Instructions de compilation

- Utiliser PlatformIO (recommandé)
- Commande : `platformio run` pour compiler, `platformio run --target upload` pour flasher
- Toutes les dépendances sont gérées automatiquement (voir platformio.ini)
- Si ajout de nouveaux menus : ajouter les chaînes dans `menus.cpp` et les déclarer dans `menus.h`

## Affectation des pins

| Fonction         | Pin Arduino |
|------------------|------------|
| 74HC595 Data     | D11        |
| 74HC595 Clock    | D12        |
| 74HC595 Latch    | D8         |
| 74HC595 Enable   | D3         |
| Appareil photo 1 | D4         |
| Appareil photo 2 | D5         |
| Ecran RST        | A0         |
| Ecran CE         | A1         |
| Ecran DC         | A2         |
| Ecran DIN        | D10        |
| Ecran CLK        | D9         |
| Neopixel         | D6         |
| Buzzer           | D7         |
| Bouton gauche    | A6 (analog)|
| Bouton droite    | A7 (analog)|
| Bouton entrée    | D2         |
| lcdLIGHT         | VCC (via R)|
| TM1637 CLK        | A3         |
| TM1637 DIO        | A4         |

**Remarque** : A6/A7 sont analogiques only, lecture bouton par `analogRead` (<512 = appuyé).

## Branchement boutons (A6/A7)
- Un côté du bouton à GND, l'autre à A6 (ou A7)
- Résistance de pull-up 10kΩ entre A6 (ou A7) et VCC

## Branchement lcdLIGHT
- Connecter la pin LIGHT de l'écran Nokia 5110 à VCC via une résistance (330Ω à 1kΩ)

## Neopixel
- Data IN du ring Neopixel sur D6

## Buzzer
- Buzzer sur D7 (GND de l'autre côté)

## TM1637 7 segments
- CLK sur A3
- DIO sur A4
- Utilisé pour afficher un compte à rebours ou des infos de menu
- Librairie : TM1637Display (PlatformIO)

## Logique boutons dans le code
- A6/A7 : `analogRead(pin) < 512` → bouton appuyé
- D2 : `digitalRead(2) == LOW` → bouton appuyé

## Dépendances PlatformIO
- Adafruit GFX Library
- Adafruit PCD8544 Nokia 5110 LCD library
- Adafruit NeoPixel (si utilisé)
- TM1637Display

---

Pour toute modification hardware, adapter le mapping dans `src/hardware.cpp` et ce README. 
# Projet Contrôle Dôme RTI

[![PlatformIO](https://img.shields.io/badge/PlatformIO-Arduino-blue?logo=platformio)](https://platformio.org/)
[![Python](https://img.shields.io/badge/Python-3.11%2B-blue?logo=python)](https://python.org/)
[![Tkinter](https://img.shields.io/badge/UI-Tkinter-informational)](https://wiki.python.org/moin/TkInter)
[![Raspberry Pi](https://img.shields.io/badge/Hardware-Raspberry%20Pi-green?logo=raspberrypi)](https://raspberrypi.org/)

---

## Présentation

Ce dépôt regroupe **deux modules complémentaires** pour le contrôle d'un dôme RTI (Reflectance Transformation Imaging) :

- **Module Arduino (Nano)** : contrôle matériel du dôme, séquençage des LEDs, déclenchement photo, interface locale (écran, joystick, menus).
- **Interface Raspberry Pi (Python)** : interface graphique tactile pour piloter le dôme, gérer les acquisitions, centraliser et exporter les photos.

---

## Structure du dépôt

```
Code_Pompei_Dome/
├── Module_controle_dome/      # Code Arduino (PlatformIO)
│   ├── src/                  # Sources C++ (main.cpp, menus.cpp, ...)
│   ├── lib/                  # Librairies Arduino (TM1637, ...)
│   ├── include/              # Headers partagés
│   ├── test/                 # Tests unitaires PlatformIO
│   ├── platformio.ini        # Config PlatformIO
│   └── README.md             # Doc détaillée Arduino
├── Interface_RaspberryPi/    # Interface graphique Python (Tkinter)
│   ├── main.py               # Menu principal
│   ├── acquisition.py        # Fenêtre nouvelle acquisition
│   ├── projects.py           # Gestion des acquisitions
│   ├── utils.py              # Fonctions utilitaires
│   ├── icons/                # Icônes PNG pour l'UI
│   ├── acquisitions/         # Dossiers d'acquisitions photo
│   ├── dev_plans/            # Plans de dev incrémentaux
│   └── README.md             # Doc détaillée interface Pi
├── doc/                      # Documentation générale, roadmap, images
│   ├── roadmap.md            # Vision, PRD, specs
│   ├── Logo_Mercurio.png     # Logo projet
│   └── Pinout-Arduino-Nano-low-resolution.jpg # Schéma pinout
└── platformio.ini            # (raccourci) Config PlatformIO racine
```

---

## Fonctionnalités principales

### 1. **Module Arduino**
- Contrôle séquentiel de 24 LEDs haute puissance (74HC595)
- Déclenchement appareil photo (optocoupleur)
- Interface locale : écran Nokia 5110, joystick, menus (tests, acquisition, réglages)
- Sécurité : extinction auto des LEDs (>10s)
- Mode autonome ou piloté par série (Raspberry Pi)
- Librairies : Adafruit GFX, PCD8544, NeoPixel, TM1637Display

### 2. **Interface Raspberry Pi**
- UI tactile (Tkinter, 800x480) adaptée écran officiel Pi 7"
- Menu principal : nouvelle acquisition, gestion des acquisitions
- Paramétrage : mur/colonne/ligne, sauvegarde/restauration auto
- Centralisation, renommage, export USB des photos
- Vérification intégrité des acquisitions (à venir)
- Style sombre, icônes PNG, navigation simple

---

## Installation rapide

### Arduino (PlatformIO)
```sh
cd Module_controle_dome
platformio run                # Compiler
platformio run --target upload # Flasher sur Arduino Nano
```
Toutes les dépendances sont gérées dans `platformio.ini`.

### Interface Raspberry Pi (Python)
- **Prérequis** : Python 3.11+, Tkinter, PIL, gphoto2
- Voir [Interface_RaspberryPi/README.md](Interface_RaspberryPi/README.md) pour l'installation détaillée (Mac ARM/conda ou Pi/Linux).

```sh
cd Interface_RaspberryPi
python main.py
```

---

## Documentation & ressources

- [doc/roadmap.md](doc/roadmap.md) : vision, PRD, specs détaillées
- [Module_controle_dome/README.md](Module_controle_dome/README.md) : doc Arduino, mapping pins, compilation
- [Interface_RaspberryPi/README.md](Interface_RaspberryPi/README.md) : doc interface graphique, installation
- [doc/Pinout-Arduino-Nano-low-resolution.jpg](doc/Pinout-Arduino-Nano-low-resolution.jpg) : schéma pinout Arduino
- [doc/Logo_Mercurio.png](doc/Logo_Mercurio.png) : logo projet

---

## Auteurs / Contact
- [Eloi Malet](mailto:eloi.malet@protonmail.com)
- [Projet Mercurio / Pompei]

---

## Licence
Projet open-source, diffusion académique et makers. Voir chaque sous-module pour détails. 
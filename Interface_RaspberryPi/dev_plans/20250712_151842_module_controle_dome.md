# Dev Plan Incrémental — Module de contrôle du dôme (Arduino)

## Phase 1 — Interface autonome minimale (écran + joystick, menu de base)
- [x] Initialiser l'écran Nokia 5110 (PCD8544 via Adafruit GFX/PCD8544)
- [x] Initialiser le joystick (4 directions + bouton)
- [x] Afficher un menu minimal sur l'écran (navigation avec joystick)
- [x] Boucle principale : navigation menu, feedback visuel
- [ ] (Option) Feedback LED RGB/buzzer sur action

## Phase 2 — Contrôle séquentiel des LEDs
- [ ] Contrôle des 24 LEDs via registre à décalage (séquence, allumage, extinction)
- [ ] Sécurité : extinction auto après 10s
- [ ] Menu de test LEDs (navigation via joystick)

## Phase 3 — Déclenchement appareil photo
- [ ] Contrôle optocoupleur (demi-pression/pression)
- [ ] Menu de déclenchement photo
- [ ] Feedback écran sur prise de vue

## Phase 4 — Mode série (contrôle par Raspberry Pi)
- [ ] Interface série : commandes reçues, feedback écran
- [ ] Documentation des messages série

## Phase 5 — Bonus
- [ ] Gestion du ring Neopixel (si pin dispo)
- [ ] Bouton supplémentaire (fonction à définir)

---

## Détail Phase 1 (implémentée)

- Utilisation des librairies Adafruit_GFX et Adafruit_PCD8544 pour l'écran Nokia 5110
- Joystick : 4 directions + bouton (entrées analogiques/digitales)
- Menu minimal :
    - "Test LEDs"
    - "Déclenchement photo"
    - "Réglages"
- Navigation :
    - Joystick haut/bas pour naviguer
    - Bouton joystick pour valider
- Feedback visuel sur l'écran (inversion, flèche, etc.)
- Structure de code modulaire pour faciliter l'ajout des phases suivantes

---

**Prochaine étape :** Phase 2 (contrôle LEDs) 
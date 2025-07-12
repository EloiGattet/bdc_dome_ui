#include "reglages.h"
#include <EEPROM.h>
#include "hardware.h"

const Reglages reglageDefaut = {2000, 500, 50, true, true, 0xBEEF};
Reglages reglage;

extern const char* reglageItems[];
extern const uint8_t reglageLength;

void chargerReglages() {
  EEPROM.get(0, reglage);
  if (reglage.crc != 0xBEEF) {
    reglage = reglageDefaut;
    EEPROM.put(0, reglage);
  }
}
void sauverReglages() {
  reglage.crc = 0xBEEF;
  EEPROM.put(0, reglage);
}

void drawReglageMenu(uint8_t idx) {
  // ... (le code complet sera déplacé ici depuis main.cpp)
}

void drawEditValue(const char* titre, int value, const char* unite, bool showArrows, int editIdx) {
  // ... (le code complet sera déplacé ici depuis main.cpp)
}

void drawEditBool(const char* titre, bool value) {
  // ... (le code complet sera déplacé ici depuis main.cpp)
} 
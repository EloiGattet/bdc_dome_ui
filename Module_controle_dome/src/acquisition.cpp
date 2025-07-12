#include "acquisition.h"
#include "hardware.h"
#include "reglages.h"
#include "affichage.h"

extern const char* acquisitionItems[];
extern const uint8_t acquisitionLength;

AcquisitionParams params = {120, 500};

void drawAcquisitionMenu(uint8_t idx) {
  display.clearDisplay();
  display.setTextColor(BLACK);
  drawMenuTitle("Acquisition");
  display.setCursor(0, 12);
  display.print("LED:"); display.print(params.tempsParLedMs); display.print("ms");
  display.setCursor(0, 22);
  display.print("Att:"); display.print(params.tempsAttenteMs); display.print("ms");
  display.setCursor(0, 34);
  if (idx == 0) display.print(">Ok   "); else display.print(" Ok   ");
  if (idx == 1) display.print("> Retour"); else display.print("  Retour");
  display.display();
}

void acquisitionCountdown() {
  // ... (le code complet sera déplacé ici depuis main.cpp)
} 
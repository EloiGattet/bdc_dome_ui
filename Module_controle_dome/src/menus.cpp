#include <Arduino.h>
#include "menus.h"

const char* menuItems[] = {"Acquisition", "Tests", "Reglages"};
const unsigned char menuLength = sizeof(menuItems) / sizeof(menuItems[0]);

const char* acquisitionItems[] = {"Ok", "Retour"};
const unsigned char acquisitionLength = sizeof(acquisitionItems) / sizeof(acquisitionItems[0]);

const char* testItems[] = {"Dome", "Photo", "Countdown", "Neopixel", "Buzzer", "Retour"};
const unsigned char testLength = sizeof(testItems) / sizeof(testItems[0]);

const char* reglageItems[] = {
  "Tps/led",
  "Tps stab",
  "Contraste",
  "Buzzer",
  "Neopixel",
  "Retour"
};
const unsigned char reglageLength = sizeof(reglageItems) / sizeof(reglageItems[0]); 
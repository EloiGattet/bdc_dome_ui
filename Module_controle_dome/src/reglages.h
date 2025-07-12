#ifndef REGLAGES_H
#define REGLAGES_H
#include <Arduino.h>

struct Reglages {
  uint16_t tempsParLedMs;
  uint16_t tempsStabLedMs;
  uint8_t contraste;
  bool buzzerOn;
  bool neopixelOn;
  uint16_t crc;
};

extern Reglages reglage;
extern const Reglages reglageDefaut;

void chargerReglages();
void sauverReglages();
void drawReglageMenu(uint8_t idx);
void drawEditValue(const char* titre, int value, const char* unite, bool showArrows, int editIdx);
void drawEditBool(const char* titre, bool value);

#endif // REGLAGES_H 
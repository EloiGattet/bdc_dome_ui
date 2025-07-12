#ifndef AFFICHAGE_H
#define AFFICHAGE_H
#include <Arduino.h>

extern const char* menuItems[];
extern const uint8_t menuLength;

void drawMenuTitle(const char* titre);
void drawMenu(uint8_t idx);
void neopixelBootAnimation();
void splashScreen();

#endif // AFFICHAGE_H 
#ifndef HARDWARE_H
#define HARDWARE_H
#include <Arduino.h>
#include <Adafruit_PCD8544.h>
#include <Adafruit_NeoPixel.h>
#include <TM1637Display.h>

#define NEOPIXEL_NUM 16
extern const int dataPin;
extern const int clockPin;
extern const int latchPin;
extern const int enablePin;
extern const int shutterHalfPin;
extern const int shutterFullPin;
extern const int lcdRST;
extern const int lcdCE;
extern const int lcdDC;
extern const int lcdDIN;
extern const int lcdCLK;
extern const int neopixelPin;
extern const int buzzerPin;
extern const int boutonGauche;
extern const int boutonDroite;
extern const int boutonEntree;
#define TM1637_CLK A3
#define TM1637_DIO A4

extern Adafruit_PCD8544 display;
extern Adafruit_NeoPixel ring;
extern TM1637Display display7seg;

void initIO();

#endif // HARDWARE_H 
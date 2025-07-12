#include "hardware.h"

const int dataPin   = 11;
const int clockPin  = 12;
const int latchPin  = 8;
const int enablePin = 3;
const int shutterHalfPin = 4;
const int shutterFullPin = 5;
const int lcdRST = A0;
const int lcdCE  = A1;
const int lcdDC  = A2;
const int lcdDIN = 10;
const int lcdCLK = 9;
const int neopixelPin = 6;
const int buzzerPin = 7;
const int boutonGauche = A6;
const int boutonDroite = A7;
const int boutonEntree = 2;

Adafruit_PCD8544 display(lcdCLK, lcdDIN, lcdDC, lcdCE, lcdRST);
Adafruit_NeoPixel ring(NEOPIXEL_NUM, neopixelPin, NEO_GRB + NEO_KHZ800);
TM1637Display display7seg(TM1637_CLK, TM1637_DIO);

void initIO() {
  Serial.begin(9600);
  Serial.println("Init OK");
  pinMode(dataPin,   OUTPUT);
  pinMode(clockPin,  OUTPUT);
  pinMode(latchPin,  OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(shutterHalfPin, OUTPUT);
  pinMode(shutterFullPin, OUTPUT);
  pinMode(lcdRST, OUTPUT);
  pinMode(lcdCE,  OUTPUT);
  pinMode(lcdDC,  OUTPUT);
  pinMode(lcdDIN, OUTPUT);
  pinMode(lcdCLK, OUTPUT);
  pinMode(neopixelPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(boutonEntree, INPUT_PULLUP);
  // A6/A7 analogiques : pas de pinMode
} 
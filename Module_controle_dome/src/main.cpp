#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Adafruit_NeoPixel.h>
#include <TM1637Display.h>
#include <EEPROM.h>
#include "logo.h"
#include "utils.h"

// --- Définition des pins ---
#define NEOPIXEL_NUM 16
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
const int boutonGauche = A6; // analogique
const int boutonDroite = A7; // analogique
const int boutonEntree = 2;  // digital
#define TM1637_CLK A3
#define TM1637_DIO A4

Adafruit_PCD8544 display(lcdCLK, lcdDIN, lcdDC, lcdCE, lcdRST);
Adafruit_NeoPixel ring(NEOPIXEL_NUM, neopixelPin, NEO_GRB + NEO_KHZ800);
TM1637Display display7seg(TM1637_CLK, TM1637_DIO);


void initIO() {
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

// --- Menu principal ---
const char* menuItems[] = {"Acquisition", "Tests", "Reglages"};
const uint8_t menuLength = sizeof(menuItems) / sizeof(menuItems[0]);
uint8_t menuIndex = 0;
uint8_t lastMenuIndex = 255;

void drawMenu(uint8_t idx) {
  display.clearDisplay();
  display.setTextColor(BLACK);
  // Affiche le titre sur 1 ou 2 lignes selon la largeur
  display.setCursor(0, 0);
  display.setTextSize(1);
  if (display.width() >= 70) {
    display.print("Dome Pompei");
    display.setCursor(0, 10);
  } else {
    display.print("Dome");
    display.setCursor(0, 10);
    display.print("Pompei");
    display.setCursor(0, 20);
  }
  // Affiche les entrées du menu
  for (uint8_t i = 0; i < menuLength; i++) {
    display.setCursor(0, 20 + i * 10);
    if (i == idx) {
      display.print("> ");
      display.print(menuItems[i]);
    } else {
      display.print("  ");
      display.print(menuItems[i]);
    }
  }
  display.display();
}

void splashScreen() {
  display.clearDisplay();
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.drawBitmap(0, 0, epd_bitmap_Logo_Mercurio, 84, 48, BLACK);
  display.display();
  delay(2000);
}

// --- Paramètres d'acquisition (valeurs par défaut, à remplacer par lecture EEPROM plus tard) ---
struct AcquisitionParams {
  uint16_t tempsParLedMs = 120; // ms
  uint16_t tempsAttenteMs = 500; // ms
};
AcquisitionParams params;

// --- Structure des réglages persistants ---
struct Reglages {
  uint16_t tempsParLedMs;      // 2s par défaut
  uint16_t tempsStabLedMs;    // 500ms par défaut
  uint8_t contraste;          // 50 par défaut
  bool buzzerOn;              // true par défaut
  bool neopixelOn;            // true par défaut
  uint16_t crc;               // CRC simple pour détection init
};

const Reglages reglageDefaut = {2000, 500, 50, true, true, 0xBEEF};
Reglages reglage;

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

// --- Affichage du sous-menu Acquisition (avec Retour) ---
const char* acquisitionItems[] = {"Ok", "Retour"};
const uint8_t acquisitionLength = sizeof(acquisitionItems) / sizeof(acquisitionItems[0]);
void drawAcquisitionMenu(uint8_t idx) {
  display.clearDisplay();
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.print("Acquisition");
  display.setCursor(0, 10);
  display.print("LED: ");
  display.print(params.tempsParLedMs);
  display.print("ms");
  display.setCursor(0, 20);
  display.print("Attente: ");
  display.print(params.tempsAttenteMs);
  display.print("ms");
  for (uint8_t i = 0; i < acquisitionLength; i++) {
    display.setCursor(0, 35 + i * 10);
    if (idx == i) display.print("> "); else display.print("  ");
    display.print(acquisitionItems[i]);
  }
  display.display();
}

// --- Séquence de compte à rebours acquisition ---
void acquisitionCountdown() {
  // Affichage message sur l'écran
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Acquisition en cours");
  display.setCursor(0, 10);
  display.print("Appuyez pour annuler");
  display.display();

  for (int s = 4; s > 0; s--) {
    // Affiche le chiffre sur le 4-digit display
    display7seg.showNumberDec(s, true);
    // Pulse le ring en rouge (fade out)
    for (int b = 255; b >= 0; b -= 8) {
      for (int i = 0; i < NEOPIXEL_NUM; i++) {
        ring.setPixelColor(i, ring.Color(b, 0, 0));
      }
      ring.show();
      delay(1000/32); // fade rapide
      // Si bouton appuyé, annule
      if (digitalRead(boutonEntree) == LOW) {
        ring.clear(); ring.show();
        display7seg.clear();
        return; // Annulation
      }
    }
    // Pause 1s totale
    unsigned long t0 = millis();
    while (millis() - t0 < 1000) {
      if (digitalRead(boutonEntree) == LOW) {
        ring.clear(); ring.show();
        display7seg.clear();
        return; // Annulation
      }
      delay(10);
    }
  }
  ring.clear(); ring.show();
  display7seg.clear();
}

// --- Sous-menu Tests ---
const char* testItems[] = {"Photo", "Countdown", "Neopixel", "Buzzer", "Retour"};
const uint8_t testLength = sizeof(testItems) / sizeof(testItems[0]);

void drawTestMenu(uint8_t idx) {
  display.clearDisplay();
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.print("Tests");
  for (uint8_t i = 0; i < testLength; i++) {
    display.setCursor(0, 10 + i * 10);
    if (i == idx) display.print("> "); else display.print("  ");
    display.print(testItems[i]);
  }
  display.display();
}

void testAppareilPhoto() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Déclenchement boitier");
  display.display();
  digitalWrite(shutterHalfPin, HIGH);
  delay(200);
  digitalWrite(shutterFullPin, HIGH);
  delay(500);
  digitalWrite(shutterHalfPin, LOW);
  digitalWrite(shutterFullPin, LOW);
  delay(500);
}

void testCountdownCentiemes() {
  unsigned long start = millis();
  unsigned long duration = 5000;
  while (millis() - start < duration) {
    unsigned long elapsed = millis() - start;
    int dixiemes = (elapsed / 10) % 100; // centièmes
    int secondes = (duration - elapsed) / 1000;
    int centiemes = ((duration - elapsed) % 1000) / 10;
    // Format XX:YY
    uint8_t digits[4];
    digits[0] = secondes / 10;
    digits[1] = secondes % 10;
    digits[2] = centiemes / 10;
    digits[3] = centiemes % 10;
    // Affiche avec les deux points
    display7seg.showNumberDecEx(digits[0]*1000 + digits[1]*100 + digits[2]*10 + digits[3], 0b01000000, true);
    delay(10);
  }
  display7seg.clear();
}

void testNeopixelRainbow() {
  unsigned long start = millis();
  while (millis() - start < 3000) {
    for (int i = 0; i < NEOPIXEL_NUM; i++) {
      uint32_t color = ring.ColorHSV((millis() / 5 + i * (65536 / NEOPIXEL_NUM)) % 65536);
      ring.setPixelColor(i, color);
    }
    ring.show();
    delay(20);
  }
  ring.clear();
  ring.show();
}

// --- Code sonnerie Nokia (issu de nokia.ino, adapté) ---
#define NOTE_E5  659
#define NOTE_D5  587
#define NOTE_FS4 370
#define NOTE_GS4 415
#define NOTE_CS5 554
#define NOTE_B4  494
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_A4  440
#define NOTE_CS4 277
#define NOTE_B4  494
#define NOTE_A4  440
#define NOTE_E4  330
#define NOTE_A4  440
#define REST      0

int melody[] = {
  NOTE_E5, 8, NOTE_D5, 8, NOTE_FS4, 4, NOTE_GS4, 4,
  NOTE_CS5, 8, NOTE_B4, 8, NOTE_D4, 4, NOTE_E4, 4,
  NOTE_B4, 8, NOTE_A4, 8, NOTE_CS4, 4, NOTE_E4, 4,
  NOTE_A4, 2,
};
int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int tempo = 180;

void testBuzzerNokia() {
  int wholenote = (60000 * 4) / tempo;
  int divider = 0, noteDuration = 0;
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
    divider = melody[thisNote + 1];
    if (divider > 0) {
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5;
    }
    tone(buzzerPin, melody[thisNote], noteDuration * 0.9);
    delay(noteDuration);
    noTone(buzzerPin);
  }
}

const char* reglageItems[] = {
  "Temps LED",
  "Stab LED",
  "Contraste",
  "Buzzer",
  "Neopixel",
  "Retour"
};
const uint8_t reglageLength = sizeof(reglageItems) / sizeof(reglageItems[0]);

void drawReglageMenu(uint8_t idx) {
  display.clearDisplay();
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.print("Réglages");
  for (uint8_t i = 0; i < reglageLength; i++) {
    display.setCursor(0, 10 + i * 8);
    if (i == idx) display.print("> "); else display.print("  ");
    display.print(reglageItems[i]);
    if (i == 0) { display.print(": "); display.print(reglage.tempsParLedMs); display.print("ms"); }
    if (i == 1) { display.print(": "); display.print(reglage.tempsStabLedMs); display.print("ms"); }
    if (i == 2) { display.print(": "); display.print(reglage.contraste); }
    if (i == 3) { display.print(": "); display.print(reglage.buzzerOn ? "ON" : "OFF"); }
    if (i == 4) { display.print(": "); display.print(reglage.neopixelOn ? "ON" : "OFF"); }
  }
  display.display();
}

void setup() {
  initIO();
  chargerReglages();
  display.begin();
  delay(100);
  display.setContrast(reglage.contraste);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  splashScreen();
  drawMenu(menuIndex);
}

void loop() {
  static bool lastGauche = false, lastDroite = false, lastEntree = false;
  static bool inAcquisitionMenu = false;
  static uint8_t acquisitionIdx = 0;
  static bool inTestMenu = false;
  static uint8_t testIdx = 0;
  static bool inReglageMenu = false;
  static uint8_t reglageIdx = 0;
  int gauche = analogRead(boutonGauche) < 512;
  int droite = analogRead(boutonDroite) < 512;
  int entree = digitalRead(boutonEntree) == LOW;
  bool menuChanged = false;

  if (!inAcquisitionMenu && !inTestMenu && !inReglageMenu) {
    // Navigation menu principal
    if (gauche && !lastGauche && menuIndex > 0) { menuIndex--; menuChanged = true; }
    if (droite && !lastDroite && menuIndex < menuLength-1) { menuIndex++; menuChanged = true; }
    if (entree && !lastEntree) {
      if (menuIndex == 0) { // Acquisition
        inAcquisitionMenu = true;
        acquisitionIdx = 0;
        drawAcquisitionMenu(acquisitionIdx);
      }
      if (menuIndex == 1) { // Tests
        inTestMenu = true;
        testIdx = 0;
        drawTestMenu(testIdx);
      }
      if (menuIndex == 2) { // Réglages
        inReglageMenu = true;
        reglageIdx = 0;
        drawReglageMenu(reglageIdx);
      }
      // TODO: autres sous-menus
      menuChanged = true;
    }
    lastGauche = gauche;
    lastDroite = droite;
    lastEntree = entree;
    if ((menuChanged || menuIndex != lastMenuIndex) && !inAcquisitionMenu && !inTestMenu && !inReglageMenu) {
      drawMenu(menuIndex);
      lastMenuIndex = menuIndex;
    }
  } else if (inAcquisitionMenu) {
    // Navigation sous-menu Acquisition
    if (gauche && !lastGauche && acquisitionIdx > 0) { acquisitionIdx--; drawAcquisitionMenu(acquisitionIdx); }
    if (droite && !lastDroite && acquisitionIdx < acquisitionLength-1) { acquisitionIdx++; drawAcquisitionMenu(acquisitionIdx); }
    if (entree && !lastEntree) {
      if (acquisitionIdx == 0) {
        acquisitionCountdown();
        inAcquisitionMenu = false;
        drawMenu(menuIndex);
      } else if (acquisitionIdx == 1) {
        inAcquisitionMenu = false;
        drawMenu(menuIndex);
      }
    }
    lastGauche = gauche;
    lastDroite = droite;
    lastEntree = entree;
  } else if (inTestMenu) {
    // Navigation sous-menu Tests
    if (gauche && !lastGauche && testIdx > 0) { testIdx--; drawTestMenu(testIdx); }
    if (droite && !lastDroite && testIdx < testLength-1) { testIdx++; drawTestMenu(testIdx); }
    if (entree && !lastEntree) {
      if (testIdx == 0) { // Photo
        testAppareilPhoto();
        drawTestMenu(testIdx);
      } else if (testIdx == 1) { // Countdown
        testCountdownCentiemes();
        drawTestMenu(testIdx);
      } else if (testIdx == 2) { // Neopixel
        testNeopixelRainbow();
        drawTestMenu(testIdx);
      } else if (testIdx == 3) { // Buzzer
        testBuzzerNokia();
        drawTestMenu(testIdx);
      } else if (testIdx == testLength-1) { // Retour
        inTestMenu = false;
        drawMenu(menuIndex);
      }
      // TODO: autres tests
    }
    lastGauche = gauche;
    lastDroite = droite;
    lastEntree = entree;
  } else if (inReglageMenu) {
    // Navigation menu réglages
    if (gauche && !lastGauche && reglageIdx > 0) { reglageIdx--; drawReglageMenu(reglageIdx); }
    if (droite && !lastDroite && reglageIdx < reglageLength-1) { reglageIdx++; drawReglageMenu(reglageIdx); }
    if (entree && !lastEntree) {
      if (reglageIdx == 0) { // Temps LED
        reglage.tempsParLedMs += 100;
        if (reglage.tempsParLedMs > 5000) reglage.tempsParLedMs = 100;
        sauverReglages();
        drawReglageMenu(reglageIdx);
      } else if (reglageIdx == 1) { // Stab LED
        reglage.tempsStabLedMs += 50;
        if (reglage.tempsStabLedMs > 2000) reglage.tempsStabLedMs = 50;
        sauverReglages();
        drawReglageMenu(reglageIdx);
      } else if (reglageIdx == 2) { // Contraste
        reglage.contraste += 5;
        if (reglage.contraste > 100) reglage.contraste = 10;
        display.setContrast(reglage.contraste);
        sauverReglages();
        drawReglageMenu(reglageIdx);
      } else if (reglageIdx == 3) { // Buzzer
        reglage.buzzerOn = !reglage.buzzerOn;
        sauverReglages();
        drawReglageMenu(reglageIdx);
      } else if (reglageIdx == 4) { // Neopixel
        reglage.neopixelOn = !reglage.neopixelOn;
        sauverReglages();
        drawReglageMenu(reglageIdx);
      } else if (reglageIdx == reglageLength-1) { // Retour
        inReglageMenu = false;
        drawMenu(menuIndex);
      }
    }
    lastGauche = gauche;
    lastDroite = droite;
    lastEntree = entree;
  }
  delay(30);
}
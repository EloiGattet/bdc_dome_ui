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

// --- Menu principal ---
const char* menuItems[] = {"Acquisition", "Tests", "Reglages"};
const uint8_t menuLength = sizeof(menuItems) / sizeof(menuItems[0]);
uint8_t menuIndex = 0;
uint8_t lastMenuIndex = 255;

// --- Barre horizontale sous le titre ---
void drawMenuTitle(const char* titre) {
  display.setTextSize(1);
  int16_t x1, y1; uint16_t w, h;
  display.getTextBounds(titre, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((84-w)/2, 0);
  display.print(titre);
  display.drawFastHLine(0, 9, 84, BLACK);
}

void drawMenu(uint8_t idx) {
  display.clearDisplay();
  display.setTextColor(BLACK);
  drawMenuTitle("Menu");
  for (uint8_t i = 0; i < menuLength; i++) {
    display.setCursor(0, 12 + i * 10);
    if (i == idx) {
      display.print("> ");
      display.print(menuItems[i]);
    } else {
      display.print("  ");
      display.print(menuItems[i]);
    }
  }
  display.display();
  Serial.println("Menu refresh");
}

void neopixelBootAnimation() {
  // Effet rainbow tournant
  for (int t = 0; t < 2 * NEOPIXEL_NUM; t++) {
    for (int i = 0; i < NEOPIXEL_NUM; i++) {
      uint32_t color = ring.ColorHSV((uint16_t)((i * 65536 / NEOPIXEL_NUM) + t * 4000) % 65536, 255, 128);
      ring.setPixelColor(i, color);
    }
    ring.show();
    delay(30);
  }
  // Flashs blancs rapides
  for (int f = 0; f < 3; f++) {
    for (int i = 0; i < NEOPIXEL_NUM; i++) ring.setPixelColor(i, ring.Color(80, 80, 80));
    ring.show();
    delay(60);
    ring.clear(); ring.show();
    delay(60);
  }
  // Effet "remplissage" vert
  for (int i = 0; i < NEOPIXEL_NUM; i++) {
    ring.setPixelColor(i, ring.Color(0, 80, 0));
    ring.show();
    delay(18);
  }
  delay(120);
  ring.clear(); ring.show();
}

void splashScreen() {
  display.clearDisplay();
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.drawBitmap(0, 0, epd_bitmap_Logo_Mercurio, 84, 48, BLACK);
  display.display();
  neopixelBootAnimation();
  delay(1200);
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
// --- Menu Acquisition compact ---
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

// --- Séquence de compte à rebours acquisition ---
void acquisitionCountdown() {
  // Affichage message court sur l'écran
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Acq. en cours");
  display.display();

  // Debounce bouton central avant le début
  while (digitalRead(boutonEntree) == LOW) {
    delay(10);
  }

  display7seg.setBrightness(0x08); // Pulse moins fort

  // Compte à rebours 3s avec pulse plus doux
  for (int s = 3; s > 0; s--) {
    display7seg.showNumberDec(s, true);

    // Pulse rouge plus doux et plus lent
    for (int b = 128; b >= 0; b -= 4) {
      for (int i = 0; i < NEOPIXEL_NUM; i++) {
        ring.setPixelColor(i, ring.Color(b, 0, 0));
      }
      ring.show();
      delay(1000/32); // fade plus lent
      if (digitalRead(boutonEntree) == LOW) {
        ring.clear(); ring.show();
        display7seg.clear();
        return; // Annulation
      }
    }

    // Pause pour compléter 1s
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

  // --- Séquence d'allumage des LEDs du dôme ---
  // Calcul du temps total d'acquisition
  uint32_t tempsTotalMs = reglage.tempsParLedMs * 24 + reglage.tempsStabLedMs * 24 + 1000; // +1s pour photo
  uint32_t tempsRestantMs = tempsTotalMs;
  unsigned long debut = millis();

  for (uint8_t i = 0; i < 24; i++) {
    // Affichage XX/24 centré en taille 2
    display.clearDisplay();
    display.setTextSize(2);
    char buf[8];
    sprintf(buf, "%02d/24", i+1);
    int16_t x1, y1; uint16_t w, h;
    display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((84-w)/2, 0);
    display.print(buf);
    display.setTextSize(1);
    display.display();

    // Affichage temps restant sur display7seg (mm:ss) avec clignotement du séparateur
    tempsRestantMs = tempsTotalMs - (millis() - debut);
    uint16_t secRest = tempsRestantMs / 1000;
    uint8_t min = secRest / 60;
    uint8_t sec = secRest % 60;
    bool showColon = ((sec % 2) == 0); // clignote chaque seconde
    display7seg.showNumberDecEx(min * 100 + sec, showColon ? 0b01000000 : 0, true);

    // Allume la LED i via le 74HC595
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 1 << (i % 8));
    shiftOut(dataPin, clockPin, MSBFIRST, (i < 8) ? 0 : 1 << ((i-8) % 8));
    shiftOut(dataPin, clockPin, MSBFIRST, (i < 16) ? 0 : 1 << ((i-16) % 8));
    digitalWrite(latchPin, HIGH);

    // Temps de stab
    unsigned long tStab = millis();
    while (millis() - tStab < reglage.tempsStabLedMs) {
      if (digitalRead(boutonEntree) == LOW) {
        // Extinction
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, MSBFIRST, 0);
        shiftOut(dataPin, clockPin, MSBFIRST, 0);
        shiftOut(dataPin, clockPin, MSBFIRST, 0);
        digitalWrite(latchPin, HIGH);
        display7seg.clear();
        return;
      }
      // Rafraîchit le display7seg chaque seconde
      unsigned long elapsed = millis() - debut;
      bool showColon = (((elapsed/1000) % 2) == 0);
      tempsRestantMs = tempsTotalMs - elapsed;
      secRest = tempsRestantMs / 1000;
      min = secRest / 60;
      sec = secRest % 60;
      display7seg.showNumberDecEx(min * 100 + sec, showColon ? 0b01000000 : 0, true);
      delay(5);
    }

    // Temps par LED
    unsigned long tLed = millis();
    while (millis() - tLed < reglage.tempsParLedMs) {
      if (digitalRead(boutonEntree) == LOW) {
        // Extinction
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, MSBFIRST, 0);
        shiftOut(dataPin, clockPin, MSBFIRST, 0);
        shiftOut(dataPin, clockPin, MSBFIRST, 0);
        digitalWrite(latchPin, HIGH);
        display7seg.clear();
        return;
      }
      // Rafraîchit le display7seg chaque seconde
      unsigned long elapsed = millis() - debut;
      bool showColon = (((elapsed/1000) % 2) == 0);
      tempsRestantMs = tempsTotalMs - elapsed;
      secRest = tempsRestantMs / 1000;
      min = secRest / 60;
      sec = secRest % 60;
      display7seg.showNumberDecEx(min * 100 + sec, showColon ? 0b01000000 : 0, true);
      delay(5);
    }
  }

  // Déclenchement appareil photo (2 sorties à 1 pendant 1s)
  digitalWrite(shutterHalfPin, HIGH);
  digitalWrite(shutterFullPin, HIGH);
  unsigned long tPhoto = millis();
  while (millis() - tPhoto < 1000) {
    if (digitalRead(boutonEntree) == LOW) {
      digitalWrite(shutterHalfPin, LOW);
      digitalWrite(shutterFullPin, LOW);
      display7seg.clear();
      return;
    }
    delay(5);
  }
  digitalWrite(shutterHalfPin, LOW);
  digitalWrite(shutterFullPin, LOW);

  // Extinction LEDs dôme
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  digitalWrite(latchPin, HIGH);

  display7seg.clear();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Acq. terminee");
  display.display();
  delay(800);

  // Fin : signal visuel et sonore
  // Effet vert qui tourne sur le ring
  for (int t = 0; t < 2; t++) {
    for (int i = 0; i < NEOPIXEL_NUM; i++) {
      for (int j = 0; j < NEOPIXEL_NUM; j++) {
        if (j == i) ring.setPixelColor(j, ring.Color(0, 128, 0));
        else ring.setPixelColor(j, 0);
      }
      ring.show();
      delay(40);
    }
  }
  ring.clear(); ring.show();
  // Triple beep net
  for (int b = 0; b < 3; b++) {
    digitalWrite(buzzerPin, HIGH);
    delay(60);
    digitalWrite(buzzerPin, LOW);
    delay(80);
  }
  // Message final centré
  display.clearDisplay();
  display.setTextSize(2);
  const char* okmsg = "OK!";
  int16_t x1, y1; uint16_t w, h;
  display.getTextBounds(okmsg, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((84-w)/2, 18);
  display.print(okmsg);
  display.setTextSize(1);
  display.display();
  delay(900);
}

// --- Sous-menu Tests ---
const char* testItems[] = {"Dome", "Photo", "Countdown", "Neopixel", "Buzzer", "Retour"};
const uint8_t testLength = sizeof(testItems) / sizeof(testItems[0]);
// --- Menu Tests avec scroll ---
#define TEST_MENU_VISIBLE 4
void drawTestMenu(uint8_t idx) {
  display.clearDisplay();
  display.setTextColor(BLACK);
  drawMenuTitle("Tests");
  int first = 0;
  if (idx >= TEST_MENU_VISIBLE-1) first = idx - (TEST_MENU_VISIBLE-1);
  for (uint8_t i = 0; i < TEST_MENU_VISIBLE && (first+i) < testLength; i++) {
    display.setCursor(0, 12 + i * 10);
    if ((first+i) == idx) display.print("> "); else display.print("  ");
    display.print(testItems[first+i]);
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

// --- Correction testCountdownCentiemes avec feedback écran et interruption ---
void testCountdownCentiemes() {
  unsigned long start = millis();
  unsigned long duration = 5000;
  display7seg.setBrightness(0xff);
  while (millis() - start < duration) {
    unsigned long elapsed = millis() - start;
    unsigned long remain = duration - elapsed;
    int secondes = remain / 1000;
    int centiemes = (remain % 1000) / 10;
    int value = secondes * 100 + centiemes;
    display7seg.showNumberDecEx(value, 0b01000000, true); // Affiche ":" au milieu
    // Feedback écran
    display.clearDisplay();
    display.setTextColor(BLACK);
    drawMenuTitle("Countdown");
    display.setCursor(10, 20);
    display.print(secondes); display.print("s ");
    if (centiemes < 10) display.print("0");
    display.print(centiemes);
    display.display();
    if (digitalRead(boutonEntree) == LOW) break;
    delay(10);
  }
  display7seg.clear();
  display.clearDisplay();
  display.setCursor(0, 20);
  display.print("Fin countdown");
  display.display();
  delay(200);
}

// --- Correction testNeopixelRainbow avec feedback écran et interruption (debounce boutonEntree) ---
void testNeopixelRainbow() {
  unsigned long start = millis();
  bool lastEntreeState = HIGH;
  unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 50;
  while (millis() - start < 3000) {
    for (int i = 0; i < NEOPIXEL_NUM; i++) {
      uint32_t color = ring.ColorHSV((millis() / 5 + i * (65536 / NEOPIXEL_NUM)) % 65536);
      ring.setPixelColor(i, color);
    }
    ring.show();
    display.clearDisplay();
    display.setTextColor(BLACK);
    drawMenuTitle("Test NeoPixel");
    display.setCursor(10, 20);
    display.print("Effet...");
    display.display();

    int entreeState = digitalRead(boutonEntree);
    if (entreeState != lastEntreeState) {
      lastDebounceTime = millis();
      lastEntreeState = entreeState;
    }
    if (entreeState == LOW && (millis() - lastDebounceTime) > debounceDelay) {
      break;
    }
    delay(20);
  }
  ring.clear();
  ring.show();
  display.clearDisplay();
  display.setCursor(0, 20);
  display.print("Fin NeoPixel");
  display.display();
  delay(200);
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

// --- Test LEDs via 74HC595 ---
void testDome() {
  display.clearDisplay();
  display.setTextColor(BLACK);
  drawMenuTitle("Test LEDs");
  for (uint8_t i = 0; i < 24; i++) {
    // Allume la LED i via le 74HC595 (exemple, à adapter selon ton schéma)
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 1 << (i % 8));
    shiftOut(dataPin, clockPin, MSBFIRST, (i < 8) ? 0 : 1 << ((i-8) % 8));
    shiftOut(dataPin, clockPin, MSBFIRST, (i < 16) ? 0 : 1 << ((i-16) % 8));
    digitalWrite(latchPin, HIGH);
    display.setCursor(0, 20);
    display.print("LED ");
    if (i+1 < 10) display.print("0");
    display.print(i+1);
    display.print("/24");
    display.display();
    unsigned long t0 = millis();
    while (millis() - t0 < 200) {
      if (digitalRead(boutonEntree) == LOW) goto fin_leds;
    }
  }
fin_leds:
  // Eteint tout
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  digitalWrite(latchPin, HIGH);
  display.clearDisplay();
  display.setCursor(0, 20);
  display.print("Fin test LEDs");
  display.display();
  delay(500);
}

const char* reglageItems[] = {
  "Tps/led",
  "Tps stab",
  "Contraste",
  "Buzzer",
  "Neopixel",
  "Retour"
};
const uint8_t reglageLength = sizeof(reglageItems) / sizeof(reglageItems[0]);

// --- Scroll générique pour menus longs ---
#define MENU_VISIBLE 4
void drawReglageMenu(uint8_t idx) {
  display.clearDisplay();
  display.setTextColor(BLACK);
  drawMenuTitle("Reglages");
  int first = 0;
  if (idx >= MENU_VISIBLE-1) first = idx - (MENU_VISIBLE-1);
  for (uint8_t i = 0; i < MENU_VISIBLE && (first+i) < reglageLength; i++) {
    display.setCursor(0, 12 + i * 10);
    if ((first+i) == idx) display.print(">"); else display.print("");
    display.print(reglageItems[first+i]);
    if (first+i == 0) {
      float sec = reglage.tempsParLedMs / 1000.0f;
      char buf[8];
      dtostrf(sec, 3, 1, buf); // format "2.4"
      for (char* p = buf; *p; ++p) if (*p == '.') *p = ','; // remplace . par ,
      display.print(":"); display.print(buf); display.print("s");
    }
    if (first+i == 1) { display.print(":"); display.print(reglage.tempsStabLedMs); display.print("ms"); }
    if (first+i == 2) { display.print(":"); display.print(reglage.contraste); }
    if (first+i == 3) { display.print(":"); display.print(reglage.buzzerOn ? "ON" : "OFF"); }
    if (first+i == 4) { display.print(":"); display.print(reglage.neopixelOn ? "ON" : "OFF"); }
  }
  display.display();
}

// --- Sous-sous-menu édition réglage générique ---
void drawEditValue(const char* titre, int value, const char* unite, bool showArrows, int editIdx) {
  display.clearDisplay();
  display.setTextColor(BLACK);
  display.setTextSize(1);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(titre, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((84-w)/2, 0);
  display.print(titre);
  display.drawFastHLine(0, 9, 84, BLACK);
  display.setTextSize(2);
  char buf[12];
  if (editIdx == 0) { // tps/LED : afficher en secondes
    float sec = value / 1000.0f;
    dtostrf(sec, 3, 1, buf);
    for (char* p = buf; *p; ++p) if (*p == '.') *p = ',';
    strcat(buf, "s");
  } else {
    sprintf(buf, "%d%s", value, unite ? unite : "");
  }
  display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
  int valX = (84-w)/2;
  int arrowOffset = 12;
  if (showArrows) {
    display.setCursor(valX - arrowOffset, 18);
    display.print("<");
  }
  display.setCursor(valX, 18);
  display.print(buf);
  if (showArrows) {
    display.setCursor(valX + w, 18);
    display.print(">");
  }
  display.setTextSize(1);
  display.display();
}
void drawEditBool(const char* titre, bool value) {
  display.clearDisplay();
  display.setTextColor(BLACK);
  display.setTextSize(1);
  int16_t x1, y1; uint16_t w, h;
  display.getTextBounds(titre, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((84-w)/2, 0);
  display.print(titre);
  display.setTextSize(2);
  const char* txt = value ? "ON" : "OFF";
  display.getTextBounds(txt, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((84-w)/2, 18);
  display.print(txt);
  display.setTextSize(1);
  display.display();
}

// --- SETUP ---
void setup() {
  // Serial.begin(115200);
  // delay(100);
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
  static bool inEditValue = false;
  static uint8_t editIdx = 0;
  static int editValue = 0;
  static bool editBool = false;
  static int editMin = 0, editMax = 0, editStep = 0;
  static const char* editTitre = NULL;
  static const char* editUnite = NULL;
  int gauche = analogRead(boutonGauche) < 512;
  int droite = analogRead(boutonDroite) < 512;
  int entree = digitalRead(boutonEntree) == LOW;
  bool menuChanged = false;

  // Bouclage des menus
  if (!inAcquisitionMenu && !inTestMenu && !inReglageMenu) {
    if (gauche && !lastGauche) { menuIndex = (menuIndex == 0) ? menuLength-1 : menuIndex-1; menuChanged = true; }
    if (droite && !lastDroite) { menuIndex = (menuIndex+1) % menuLength; menuChanged = true; }
    if (entree && !lastEntree) {
      /*Serial.print("[NAV] Entrée, menuIndex: "); Serial.println(menuIndex);*/
      if (menuIndex == 0) { inAcquisitionMenu = true; acquisitionIdx = 0; drawAcquisitionMenu(acquisitionIdx); /*Serial.println("[ACQ] Entrée menu acquisition");*/ }
      if (menuIndex == 1) { inTestMenu = true; testIdx = 0; drawTestMenu(testIdx); /*Serial.println("[TEST] Entrée menu test");*/ }
      if (menuIndex == 2) { inReglageMenu = true; reglageIdx = 0; drawReglageMenu(reglageIdx); /*Serial.println("[REGLAGE] Entrée menu réglages");*/ }
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
    if (gauche && !lastGauche) { acquisitionIdx = (acquisitionIdx == 0) ? acquisitionLength-1 : acquisitionIdx-1; drawAcquisitionMenu(acquisitionIdx); /*Serial.print("[ACQ] Gauche, idx: "); Serial.println(acquisitionIdx);*/ }
    if (droite && !lastDroite) { acquisitionIdx = (acquisitionIdx+1) % acquisitionLength; drawAcquisitionMenu(acquisitionIdx); /*Serial.print("[ACQ] Droite, idx: "); Serial.println(acquisitionIdx);*/ }
    if (entree && !lastEntree) {
      /*Serial.print("[ACQ] Entrée, idx: "); Serial.println(acquisitionIdx);*/
      if (acquisitionIdx == 0) { acquisitionCountdown(); inAcquisitionMenu = false; drawMenu(menuIndex); /*Serial.println("[ACQ] Lancement acquisition");*/ }
      else if (acquisitionIdx == 1) { inAcquisitionMenu = false; drawMenu(menuIndex); /*Serial.println("[ACQ] Retour menu principal");*/ }
    }
    lastGauche = gauche;
    lastDroite = droite;
    lastEntree = entree;
  } else if (inTestMenu) {
    if (gauche && !lastGauche) { testIdx = (testIdx == 0) ? testLength-1 : testIdx-1; drawTestMenu(testIdx); /*Serial.print("[TEST] Gauche, idx: "); Serial.println(testIdx);*/ }
    if (droite && !lastDroite) { testIdx = (testIdx+1) % testLength; drawTestMenu(testIdx); /*Serial.print("[TEST] Droite, idx: "); Serial.println(testIdx);*/ }
    if (entree && !lastEntree) {
      /*Serial.print("[TEST] Entrée, idx: "); Serial.println(testIdx);*/
      if (testIdx == 1) { testAppareilPhoto(); drawTestMenu(testIdx); /*Serial.println("[TEST] Test appareil photo");*/ }
      else if (testIdx == 2) { testCountdownCentiemes(); drawTestMenu(testIdx); /*Serial.println("[TEST] Test countdown centièmes");*/ }
      else if (testIdx == 3) { testNeopixelRainbow(); drawTestMenu(testIdx); /*Serial.println("[TEST] Test Neopixel");*/ }
      else if (testIdx == 4) { testBuzzerNokia(); drawTestMenu(testIdx); /*Serial.println("[TEST] Test buzzer");*/ }
      else if (testIdx == 0) { testDome(); drawTestMenu(testIdx); }
      else if (testIdx == testLength-1) { inTestMenu = false; drawMenu(menuIndex); /*Serial.println("[TEST] Retour menu principal");*/ }
    }
    lastGauche = gauche;
    lastDroite = droite;
    lastEntree = entree;
  } else if (inReglageMenu && !inEditValue) {
    if (gauche && !lastGauche) { reglageIdx = (reglageIdx == 0) ? reglageLength-1 : reglageIdx-1; drawReglageMenu(reglageIdx); }
    if (droite && !lastDroite) { reglageIdx = (reglageIdx+1) % reglageLength; drawReglageMenu(reglageIdx); }
    if (entree && !lastEntree) {
      if (reglageIdx == 0) { // Temps LED
        inEditValue = true; editIdx = 0; editValue = reglage.tempsParLedMs; editMin = 100; editMax = 5000; editStep = 100; editTitre = "TPS/LED"; editUnite = "ms"; drawEditValue(editTitre, editValue, editUnite, true, editIdx);
      } else if (reglageIdx == 1) { // Stab LED
        inEditValue = true; editIdx = 1; editValue = reglage.tempsStabLedMs; editMin = 50; editMax = 2000; editStep = 50; editTitre = "STAB LED"; editUnite = "ms"; drawEditValue(editTitre, editValue, editUnite, true, editIdx);
      } else if (reglageIdx == 2) { // Contraste
        inEditValue = true; editIdx = 2; editValue = reglage.contraste; editMin = 10; editMax = 100; editStep = 5; editTitre = "CONTRASTE"; editUnite = NULL; drawEditValue(editTitre, editValue, editUnite, true, editIdx);
      } else if (reglageIdx == 3) { // Buzzer
        inEditValue = true; editIdx = 3; editBool = reglage.buzzerOn; editTitre = "BUZZER"; drawEditBool(editTitre, editBool);
      } else if (reglageIdx == 4) { // Neopixel
        inEditValue = true; editIdx = 4; editBool = reglage.neopixelOn; editTitre = "NEOPIXEL"; drawEditBool(editTitre, editBool);
      } else if (reglageIdx == reglageLength-1) { inReglageMenu = false; drawMenu(menuIndex); }
    }
    lastGauche = gauche;
    lastDroite = droite;
    lastEntree = entree;
  } else if (inReglageMenu && inEditValue) {
    if (editIdx <= 2) { // Edition valeur numérique
      if (gauche && !lastGauche) { editValue -= editStep; if (editValue < editMin) editValue = editMax; drawEditValue(editTitre, editValue, editUnite, true, editIdx); }
      if (droite && !lastDroite) { editValue += editStep; if (editValue > editMax) editValue = editMin; drawEditValue(editTitre, editValue, editUnite, true, editIdx); }
      if (entree && !lastEntree) {
        if (editIdx == 0) reglage.tempsParLedMs = editValue;
        else if (editIdx == 1) reglage.tempsStabLedMs = editValue;
        else if (editIdx == 2) { reglage.contraste = editValue; display.setContrast(reglage.contraste); }
        sauverReglages();
        inEditValue = false;
        drawReglageMenu(reglageIdx);
      }
    } else { // Edition booléen
      if ((gauche && !lastGauche) || (droite && !lastDroite)) { editBool = !editBool; drawEditBool(editTitre, editBool); }
      if (entree && !lastEntree) {
        if (editIdx == 3) reglage.buzzerOn = editBool;
        else if (editIdx == 4) reglage.neopixelOn = editBool;
        sauverReglages();
        inEditValue = false;
        drawReglageMenu(reglageIdx);
      }
    }
    lastGauche = gauche;
    lastDroite = droite;
    lastEntree = entree;
  }
  delay(30);
}
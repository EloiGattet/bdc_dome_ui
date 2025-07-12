#include "tests.h"
#include "hardware.h"
#include "affichage.h"
#include "menus.h"
extern const char* testItems[];
extern const uint8_t testLength;

void drawTestMenu(uint8_t idx) {
  display.clearDisplay();
  display.setTextColor(BLACK);
  drawMenuTitle("Tests");
  int first = 0;
  const int TEST_MENU_VISIBLE = 4;
  if (idx >= TEST_MENU_VISIBLE-1) first = idx - (TEST_MENU_VISIBLE-1);
  for (uint8_t i = 0; i < TEST_MENU_VISIBLE && (first+i) < testLength; i++) {
    display.setCursor(0, 10 + i * 10);
    if ((first+i) == idx) display.print("> "); else display.print("  ");
    display.print(testItems[first+i]);
  }
  display.display();
}

void testDome() {
  // Debounce : attendre que le bouton entrée soit relâché avant de commencer le test
  while (digitalRead(boutonEntree) == LOW) {
    delay(10);
  }
  display.clearDisplay();
  display.setTextColor(BLACK);
  drawMenuTitle("Test LEDs");
  for (unsigned char i = 0; i < 24; i++) {
    // Allume la LED i via le 74HC595 (exemple, à adapter selon ton schéma)
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 1 << (i % 8));
    shiftOut(dataPin, clockPin, MSBFIRST, (i < 8) ? 0 : 1 << ((i-8) % 8));
    shiftOut(dataPin, clockPin, MSBFIRST, (i < 16) ? 0 : 1 << ((i-16) % 8));
    digitalWrite(latchPin, HIGH);
    display.clearDisplay();
    display.setCursor(0, 20);
    display.print("  LED ");
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

void testAppareilPhoto() {
  display.clearDisplay();
  const char* ligne1 = "Declenchement";
  const char* ligne2 = "boitier";
  int16_t x1, y1; uint16_t w, h;

  // Ligne 1 centrée
  display.setTextSize(1);
  display.getTextBounds(ligne1, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((84-w)/2, 10);
  display.print(ligne1);

  // Ligne 2 centrée
  display.getTextBounds(ligne2, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((84-w)/2, 24);
  display.print(ligne2);

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
  display7seg.setBrightness(0xff);
  while (digitalRead(boutonEntree) == LOW) {
    delay(10);
  }
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
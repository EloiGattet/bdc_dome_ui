#include "acquisition.h"
#include "hardware.h"
#include "reglages.h"
#include "affichage.h"
#include "menus.h"
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
    unsigned long msElapsed = millis() - debut;
    bool showColon = ((msElapsed % 1000) < 200); // Allumé 200ms chaque seconde
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
      bool showColon = ((elapsed % 1000) < 200); // Allumé 200ms chaque seconde
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
      bool showColon = ((elapsed % 1000) < 200); // Allumé 200ms chaque seconde
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
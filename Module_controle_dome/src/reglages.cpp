#include <Arduino.h>
#include <EEPROM.h>
#include "hardware.h"
#include "affichage.h"
#include "reglages.h"
#include "menus.h"
extern const char* reglageItems[];
extern const uint8_t reglageLength;

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

void drawReglageMenu(uint8_t idx) {
  display.clearDisplay();
  display.setTextColor(BLACK);
  drawMenuTitle("Reglages");
  int first = 0;
  const int MENU_VISIBLE = 4;
  if (idx >= MENU_VISIBLE-1) first = idx - (MENU_VISIBLE-1);
  for (uint8_t i = 0; i < MENU_VISIBLE && (first+i) < reglageLength; i++) {
    display.setCursor(0, 10 + i * 10);
    if ((first+i) == idx) display.print(">"); else display.print(" ");
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
#include "affichage.h"
#include "hardware.h"

void drawMenuTitle(const char* titre) {
  display.setTextSize(1);
  int16_t x1, y1; uint16_t w, h;
  display.getTextBounds(titre, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((84-w)/2, 0);
  display.print(titre);
  display.drawFastHLine(0, 8, 84, BLACK);
}

void drawMenu(uint8_t idx) {
  display.clearDisplay();
  display.setTextColor(BLACK);
  drawMenuTitle("Menu");
  extern const char* menuItems[];
  extern const uint8_t menuLength;
  for (uint8_t i = 0; i < menuLength; i++) {
    display.setCursor(0, 12 + i * 10); // Décalage de 2px vers le haut
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
  for (int t = 0; t < 2 * NEOPIXEL_NUM; t++) {
    for (int i = 0; i < NEOPIXEL_NUM; i++) {
      uint32_t color = ring.ColorHSV((uint16_t)((i * 65536 / NEOPIXEL_NUM) + t * 4000) % 65536, 255, 128);
      ring.setPixelColor(i, color);
    }
    ring.show();
    delay(30);
  }
  for (int f = 0; f < 3; f++) {
    for (int i = 0; i < NEOPIXEL_NUM; i++) ring.setPixelColor(i, ring.Color(80, 80, 80));
    ring.show();
    delay(60);
    ring.clear(); ring.show();
    delay(60);
  }
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
  extern const unsigned char epd_bitmap_Logo_Mercurio[];
  display.drawBitmap(0, 0, epd_bitmap_Logo_Mercurio, 84, 48, BLACK);
  display.display();
  neopixelBootAnimation();
  delay(500);
} 
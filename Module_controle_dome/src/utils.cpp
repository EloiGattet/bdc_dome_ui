#include "utils.h"
void test_contraste(Adafruit_PCD8544* display) {
  for (uint8_t c = 0; c <= 127; c++) {
    display->setContrast(c);
    display->clearDisplay();
    display->setCursor(0, 0);
    display->print("Contraste:");
    display->setCursor(0, 10);
    display->print(c);
    display->display();
    delay(150);
  }
  while(1); // Bloque ici après le test
} 
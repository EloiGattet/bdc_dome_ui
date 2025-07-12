#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Adafruit_NeoPixel.h>
#include <TM1637Display.h>
#include <EEPROM.h>
#include "logo.h"
#include "utils.h"
#include "hardware.h"
#include "affichage.h"
#include "acquisition.h"
#include "tests.h"
#include "reglages.h"

// Déclarations globales menus (à déplacer si besoin)
const char* menuItems[] = {"Acquisition", "Tests", "Reglages"};
const uint8_t menuLength = sizeof(menuItems) / sizeof(menuItems[0]);
uint8_t menuIndex = 0;
uint8_t lastMenuIndex = 255;

// Menus secondaires (acquisition, tests, réglages)
const char* acquisitionItems[] = {"Ok", "Retour"};
const uint8_t acquisitionLength = sizeof(acquisitionItems) / sizeof(acquisitionItems[0]);

const char* testItems[] = {"Dome", "Photo", "Countdown", "Neopixel", "Buzzer", "Retour"};
const uint8_t testLength = sizeof(testItems) / sizeof(testItems[0]);

const char* reglageItems[] = {
  "Tps/led",
  "Tps stab",
  "Contraste",
  "Buzzer",
  "Neopixel",
  "Retour"
};
const uint8_t reglageLength = sizeof(reglageItems) / sizeof(reglageItems[0]);

// --- Paramètres d'acquisition (valeurs par défaut, à remplacer par lecture EEPROM plus tard) ---
// --- Structure des réglages persistants ---

// --- SETUP ---
void setup() {
  // Serial.begin(115200);
  // delay(100);
  initIO();
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
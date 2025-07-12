#ifndef ACQUISITION_H
#define ACQUISITION_H
#include <Arduino.h>

struct AcquisitionParams {
  uint16_t tempsParLedMs;
  uint16_t tempsAttenteMs;
};

extern AcquisitionParams params;

void drawAcquisitionMenu(uint8_t idx);
void acquisitionCountdown();

#endif // ACQUISITION_H 
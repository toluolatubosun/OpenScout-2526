#ifndef HC12_RECEIVER_H
#define HC12_RECEIVER_H

#include <Arduino.h>

// === HC-12 CONFIGURATION ===
extern const int HC12_BAUD_RATE;

// === FUNCTION DECLARATIONS ===
void initializeHC12();
void handleHC12Commands();

#endif // HC12_RECEIVER_H

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <LiquidCrystal.h>

// === WIFI CONFIGURATION ===
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// === WIFI STATE ===
extern volatile bool wifiConnected;

// === LCD OBJECT ===
extern LiquidCrystal lcd;

// === FUNCTION DECLARATIONS ===
void initializeWiFi();
bool isWiFiConnected();
void checkConnections();
void connectWiFi();
void updateLCDDisplay();

#endif // WIFI_MANAGER_H
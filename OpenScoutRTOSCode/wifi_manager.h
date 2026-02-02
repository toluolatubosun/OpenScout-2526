#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

// === WIFI CONFIGURATION ===
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// === WIFI STATE ===
extern volatile bool wifiConnected;

// === FUNCTION DECLARATIONS ===
void initializeWiFi();
bool isWiFiConnected();
void checkConnections();
void connectWiFi();

#endif // WIFI_MANAGER_H

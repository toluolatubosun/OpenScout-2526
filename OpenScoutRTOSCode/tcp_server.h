#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <WiFi.h>

// === TCP CONFIGURATION ===
extern const int TCP_PORT;

// === TCP STATE ===
extern WiFiServer* tcpServer;
extern WiFiClient tcpClient;
extern volatile bool clientConnected;

// === FUNCTION DECLARATIONS ===
void initializeTCPServer();
void handleTCPClient();
void sendStatusJSON();

#endif // TCP_SERVER_H

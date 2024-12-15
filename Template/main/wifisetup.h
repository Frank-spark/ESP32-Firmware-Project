#ifndef WiFiSetup_h
#define WiFiSetup_h

#include <WiFi.h>
#include <WebServer.h>

extern WebServer otaServer; // Externally declare otaServer
void setupWiFiAndWebServer();

#endif

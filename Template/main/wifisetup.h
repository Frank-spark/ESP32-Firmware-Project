
#ifndef WiFiSetup_h
#define WiFiSetup_h

#include <WiFi.h>
#include <WebServer.h>
#include <WiFiManager.h> // Include WiFiManager library

extern WebServer otaServer; // Externally declare otaServer

void setupWiFiAndWebServer();

#endif

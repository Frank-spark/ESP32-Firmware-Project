#ifndef WiFiSetup_h
#define WiFiSetup_h

#include <WiFi.h>
#include <WebServer.h>
//this will be ethernet in production code using wifi for ease of testing
extern WebServer otaServer; // Externally declare otaServer
void setupWiFiAndWebServer();

#endif

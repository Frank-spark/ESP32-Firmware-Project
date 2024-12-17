#ifndef SocketIOSetup_h
#define SocketIOSetup_h

#include <SocketIOclient.h>
#include <ArduinoJson.h>
#include <Arduino.h>

// Global instance of Socket.IO client
extern SocketIOclient socketIO;
// Global variables for firmware spicific communication in SocketioSetup.cpp
extern int brightness;
extern int floatLevel;

// Function prototypes
void setupSocketIO();
void handleSocketIO();
void registerSocketIOEvents();

// Specific Socket.IO event handling functions
void handleSetBrightness(const JsonObject& payload);
void handleSetFloatLevel(const JsonObject& payload);

// LED Update Function
void updateLEDs();

#endif

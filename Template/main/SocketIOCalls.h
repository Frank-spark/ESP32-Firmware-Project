#ifndef SocketIOCalls_h
#define SocketIOCalls_h

#include <ArduinoJson.h>
#include <Arduino.h>

extern SocketIOclient socketIO;

// Function to handle events
void setupSocketIOCalls();
void handleSocketIOCalls(const String& eventName, const JsonObject& data);

#endif

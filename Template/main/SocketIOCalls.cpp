#include "SocketIOCalls.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include "SocketIOSetup.h"
// External variables
extern int floatLevel;
extern int brightness;

void setupSocketIOCalls() {
    socketIO.onEvent([](socketIOmessageType_t type, uint8_t* payload, size_t length) {
        if (type == sIOtype_EVENT) {
            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, payload, length);
            if (error) {
                Serial.printf("[IOc] JSON parse error: %s\n", error.c_str());
                return;
            }

            String eventName = doc[0];
            JsonObject data = doc[1].as<JsonObject>();

            handleSocketIOCalls(eventName, data);
        }
    });
}

// Handle Socket.IO events
void handleSocketIOCalls(const String& eventName, const JsonVariant& payload) {
    if (eventName == "setFloatLevel") {
        int newLevel = payload["level"];
        floatLevel = constrain(newLevel, 0, 100);
        Serial.printf("[SocketIOCalls] Float level set to: %d\n", floatLevel);
    } else if (eventName == "setBrightness") {
        int newBrightness = payload["brightness"];
        brightness = constrain(newBrightness, 0, 255);
        Serial.printf("[SocketIOCalls] Brightness set to: %d\n", brightness);
    } else {
        Serial.printf("[SocketIOCalls] Unknown event: %s\n", eventName.c_str());
    }
}

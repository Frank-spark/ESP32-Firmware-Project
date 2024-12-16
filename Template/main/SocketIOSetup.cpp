#include "SocketIOSetup.h"
#include "SocketIOCalls.h"
#include <ArduinoJson.h>
#include <SocketIOclient.h>

// Ping interval
unsigned long lastPingTime = 0;
const unsigned long pingInterval = 5000;

SocketIOclient socketIO;

// Event handler
void socketIOEvent(socketIOmessageType_t type, uint8_t* payload, size_t length) {
    switch (type) {
        case sIOtype_DISCONNECT:
            Serial.println("[IOc] Disconnected!");
            break;

        case sIOtype_CONNECT:
            Serial.printf("[IOc] Connected to URL: %s\n", payload);
            socketIO.send(sIOtype_CONNECT, "/"); // Join the default namespace
            break;

        case sIOtype_EVENT: {
            Serial.println("[IOc] Event received");
            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, payload, length);
            if (error) {
                Serial.printf("[IOc] JSON Parse Error: %s\n", error.c_str());
                return;
            }

            String eventName = doc[0];
            Serial.printf("[IOc] Event Name: %s\n", eventName.c_str());
            handleSocketIOCalls(eventName, doc[1]);
            break;
        }

        case sIOtype_ACK:
            Serial.printf("[IOc] ACK received, length: %u\n", length);
            break;

        default:
            Serial.printf("[IOc] Unhandled SocketIO message type: %d\n", type);
            break;
    }
}

// Setup function for Socket.IO
void setupSocketIO() {
    Serial.println("[SocketIOSetup] Setting up Socket.IO...");

    // Begin connection
    socketIO.begin("192.168.1.221", 5000, "/socket.io/?EIO=4");

    // Register event handler
    socketIO.onEvent(socketIOEvent);

    Serial.println("[SocketIOSetup] Socket.IO setup completed.");
}

// Loop function for Socket.IO
void handleSocketIO() {
    socketIO.loop();

    // Send a ping every 5 seconds
    if (millis() - lastPingTime > pingInterval) {
        lastPingTime = millis();

        DynamicJsonDocument doc(1024);
        JsonArray array = doc.to<JsonArray>();
        array.add("ping");
        array.add("Ping from ESP32");

        String output;
        serializeJson(doc, output);

        socketIO.sendEVENT(output);
        Serial.println("[SocketIOSetup] Ping sent to the server.");
    }
}

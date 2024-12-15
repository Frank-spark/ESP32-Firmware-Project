#include "SocketIOSetup.h"
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "wifisetup.h"

// Declare the WebSocketsClient
WebSocketsClient webSocket;

// Server settings
const char* socketServerHost = "192.168.1.9"; // Replace with your server's IP
const uint16_t socketServerPort = 3000;       // Replace with your server's port

unsigned long lastPingTime = 0; // For heartbeat pings

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.println("WebSocket disconnected.");
            break;

        case WStype_CONNECTED:
            Serial.println("WebSocket connected.");
            break;

        case WStype_TEXT:
            Serial.printf("Received: %s\n", payload);
            // Ignore protocol messages like "0{...}" or "2"
            if (payload[0] == '4' && payload[1] == '2') { // Socket.IO message frame
                // Extract the JSON message from the payload
                String message = String((char *)payload + 2); // Skip "42"
                StaticJsonDocument<256> doc;
                DeserializationError error = deserializeJson(doc, message);
                if (error) {
                    Serial.print("JSON deserialization failed: ");
                    Serial.println(error.c_str());
                } else {
                    // Handle your JSON message
                    Serial.println("Parsed JSON:");
                    serializeJson(doc, Serial);
                    Serial.println();
                }
            }
            break;

        default:
            break;
    }
}
void handleWebSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.println("WebSocket disconnected.");
            break;

        case WStype_CONNECTED:
            Serial.println("WebSocket connected.");
            // No need to manually send a ping here; let the library handle it.
            break;

        case WStype_TEXT:
            Serial.printf("Received: %s\n", payload);
            break;

        case WStype_PING:
            Serial.println("Ping received.");
            break;

        case WStype_PONG:
            Serial.println("Pong received.");
            break;

        default:
            break;
    }
}

void setupSocketIO() {
    webSocket.begin(socketServerHost, socketServerPort, "/socket.io/?EIO=4&transport=websocket");
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000); // Try reconnecting every 5 seconds

    Serial.println(F("WebSocket client initialized."));
}

// Function to maintain the WebSocket connection and send periodic heartbeats
void handleSocketIO() {
    webSocket.loop();

    // Send a periodic heartbeat
    if (millis() - lastPingTime >= 20000) { // Send ping every 10 seconds
        String pingMessage = "42[\"heartbeat\",{\"status\":\"alive\"}]";
        webSocket.sendTXT(pingMessage);
        lastPingTime = millis();
        Serial.println(F("Sent heartbeat ping"));
    }
}


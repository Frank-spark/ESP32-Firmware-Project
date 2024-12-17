#include "SocketIOSetup.h"

// Global Socket.IO client
SocketIOclient socketIO;

// Ping interval variables
unsigned long lastPingTime = 0;
const unsigned long pingInterval = 5000;

// Variables for Socketio to be used in main 


// Easily Editable Section for Event-Specific Functions
void handleSetBrightness(const JsonObject& payload) {
    int newBrightness = payload["brightness"] | -1; // Default to -1 if missing
    if (newBrightness >= 0 && newBrightness <= 255) {
        brightness = newBrightness;
        Serial.printf("[SocketIO] Brightness set to: %d\n", brightness);
    } else {
        Serial.println("[SocketIO] Invalid brightness value!");
    }
}

void handleSetFloatLevel(const JsonObject& payload) {
    int newLevel = payload["level"] | -1; // Default to -1 if missing
    if (newLevel >= 0 && newLevel <= 100) {
        floatLevel = newLevel;
        Serial.printf("[SocketIO] Float level set to: %d\n", floatLevel);
    } else {
        Serial.println("[SocketIO] Invalid float level value!");
    }
}

// Main event handler for Socket.IO
void handleSocketIOEvent(const String& eventName, const JsonObject& data) {
    if (eventName == "setBrightness") {
        brightness = data["brightness"] | -1;
        brightness = constrain(brightness, 0, 255);
        Serial.printf("Brightness set to: %d\n", brightness);
        updateLEDs(); // Update LEDs after changing brightness
    } else if (eventName == "setFloatLevel") {
        floatLevel = data["level"] | -1;
        floatLevel = constrain(floatLevel, 0, 100);
        Serial.printf("Float Level set to: %d\n", floatLevel);
        updateLEDs(); // Update LEDs after changing float level
    } else {
        Serial.printf("Unknown event: %s\n", eventName.c_str());
    }
}



// Socket.IO Event Handler Function
void socketIOEvent(socketIOmessageType_t type, uint8_t* payload, size_t length) {
    //Serial.println("\n[SocketIO] ---- Event Debug Start ----");

    // Print event type and raw payload
    //Serial.printf("[SocketIO] Event Type: %d\n", type);
   // Serial.print("[SocketIO] Raw Payload: ");
    for (size_t i = 0; i < length; i++) {
        //Serial.print((char)payload[i]);
    }
    Serial.println();

    switch (type) {
        case sIOtype_EVENT: {
            //Serial.println("[SocketIO] EVENT received");

            // Parse the payload
            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, payload, length);
            if (error) {
                //Serial.printf("[SocketIO] JSON Parse Error: %s\n", error.c_str());
                return;
            }

            // Debug the parsed JSON structure
            //Serial.println("[SocketIO] Parsed JSON:");
            serializeJsonPretty(doc, Serial);
            Serial.println();

            // Check if the payload is an array
            if (doc.is<JsonArray>()) {
                String eventName = doc[0].as<String>();
                //Serial.printf("[SocketIO] Event Name: %s\n", eventName.c_str());

                // Handle cases where the second element is a JsonObject
                if (doc[1].is<JsonObject>()) {
                    JsonObject data = doc[1].as<JsonObject>();
                   // Serial.println("[SocketIO] Data Object:");
                    serializeJsonPretty(data, Serial);
                    Serial.println();

                    // Pass the event and data to the handler
                    handleSocketIOEvent(eventName, data);
                }
                // Handle cases where the second element is a String
                else if (doc[1].is<const char*>()) {
                    const char* data = doc[1];
                    //Serial.printf("[SocketIO] Data String: %s\n", data);

                    // Optionally call the handler for string-type data
                    handleSocketIOEvent(eventName, JsonObject());
                }
                // Handle other data types
                else {
                    //Serial.println("[SocketIO] Second element is not recognized!");
                }
            } else {
                //Serial.println("[SocketIO] Payload is not a JsonArray!");
            }
            break;
        }

        case sIOtype_DISCONNECT:
            Serial.println("[SocketIO] DISCONNECT received!");
            break;

        case sIOtype_CONNECT:
            Serial.printf("[SocketIO] CONNECTED to URL: %s\n", payload);
            socketIO.send(sIOtype_CONNECT, "/"); // Join default namespace
            break;

        default:
            Serial.printf("[SocketIO] Unhandled event type: %d\n", type);
            break;
    }

    //Serial.println("[SocketIO] ---- Event Debug End ----\n");
}



// Register Socket.IO Events
void registerSocketIOEvents() {
    socketIO.onEvent(socketIOEvent);
}

// Setup Socket.IO
void setupSocketIO() {
    Serial.println("[SocketIOSetup] Setting up Socket.IO...");

    // Start the Socket.IO client
    socketIO.begin("192.168.1.221", 5000, "/socket.io/?EIO=4");

    // Register event handlers
    registerSocketIOEvents();

    Serial.println("[SocketIOSetup] Socket.IO setup completed.");
}

// Handle Socket.IO Communication
void handleSocketIO() {
    socketIO.loop(); // Maintain the Socket.IO connection

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
        Serial.println("[SocketIO] Ping sent to the server.");
    }
}

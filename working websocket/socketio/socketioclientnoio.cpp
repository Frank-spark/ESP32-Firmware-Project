#include <WiFi.h>
#include <SocketIoClient.h>  // Include the Socket.IO client header

const char* ssid     = "Special Projects-5GHz";  // WiFi SSID
const char* password = "sprojects1!";  // WiFi password
char host[] = "192.168.1.221";  // Socket.IO server IP
int port = 5000;  // Socket.IO server port

SocketIoClient socketIoClient;  // Socket.IO client object

// Timer variables for periodic tasks
unsigned long previousMillis = 0;
const long pingInterval = 2000;  // Ping interval in milliseconds

void setup() {
    // Initialize serial for debugging
    Serial.begin(115200);
    delay(10);

    // Connect to WiFi
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);  // Begin WiFi connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");  // Wait for connection
    }
    Serial.println("Connected to WiFi.");

    // Initialize Socket.IO connection
    socketIoClient.begin(host, port);  // Connect to Socket.IO server

    // Listen for 'hello' event from server
    socketIoClient.on("hello", helloEvent);  // Set callback function for hello event
}

void sendPing() {
    // Emit a properly formatted ping message as a JSON string
    String jsonPayload = "{\"message\":\"Ping from ESP32\"}";
    socketIoClient.emit("ping", jsonPayload.c_str());  // Send JSON message to server
    Serial.println("Ping sent to server");
}

// Callback function for hello event
void helloEvent(const char * payload, size_t length) {
    Serial.print("Hello event received from server: ");
    Serial.println(payload);  // Print the hello message from the server

    // Send a 'pong_received' message back to the server in proper format
    String jsonResponse = "{\"message\":\"Pong message received by ESP32\"}";
    socketIoClient.emit("pong_received", jsonResponse.c_str());  // Send acknowledgment back to server
    Serial.println("Pong received message sent back to server");
}

void loop() {
    // Maintain the Socket.IO connection
    socketIoClient.loop();

    // Get current time
    unsigned long currentMillis = millis();

    // Send ping message at regular intervals
    if (currentMillis - previousMillis >= pingInterval) {
        previousMillis = currentMillis;  // Update the last ping time
        sendPing();  // Send ping to server
    }
}

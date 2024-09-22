#include <WiFi.h>
#include <WebSocketClient.h>

const char* ssid     = "NETGEAR60";
const char* password = "";
char path[] = "/";
char host[] = "192.168.1.21:5000";

// WebSocket client and WiFi client
WebSocketClient webSocketClient;
WiFiClient client;

// Pins for buttons and shift register (SN74HC595)
const int buttonPin1 = 18;  // GPIO for Key 1
const int buttonPin2 = 19;  // GPIO for Key 2
const int buttonPin3 = 21;  // GPIO for Key 3
const int buttonPin4 = 23;  // GPIO for Key 4

// Corrected pins for the SN74HC595 shift register
#define DATA_PIN   13  // SER pin on SN74HC595
#define CLOCK_PIN  27  // SRCK pin on SN74HC595
#define LATCH_PIN  14  // RCK pin on SN74HC595
#define OE_PIN     4   // Output Enable pin on SN74HC595

// Store the current state of the relays (each bit represents one relay)
// Using 3 shift registers (sr_count = 3), hence 3 bytes of relay states
byte relayState[3] = { B00000000, B00000000, B00000000 };  // 3 shift registers, initialize all relays OFF

// Variables to store button states
int buttonState1 = HIGH;
int buttonState2 = HIGH;
int buttonState3 = HIGH;
int buttonState4 = HIGH;

// Variables to store previous button states
int lastButtonState1 = HIGH;
int lastButtonState2 = HIGH;
int lastButtonState3 = HIGH;
int lastButtonState4 = HIGH;

// Timing variables for ping
unsigned long previousMillis = 0;
const long pingInterval = 5000; // Send ping every 5 seconds

void setup() {
    Serial.begin(115200);
    delay(10);

    // Initialize button pins as input with pull-up resistors
    pinMode(buttonPin1, INPUT_PULLUP);
    pinMode(buttonPin2, INPUT_PULLUP);
    pinMode(buttonPin3, INPUT_PULLUP);
    pinMode(buttonPin4, INPUT_PULLUP);

    // Initialize shift register pins
    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);
    pinMode(OE_PIN, OUTPUT);
    
    digitalWrite(OE_PIN, LOW);  // Ensure the output enable pin is LOW to enable the relays
    digitalWrite(LATCH_PIN, LOW);  // Make sure the latch is low initially

    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("WiFi connected, IP address: ");
    Serial.println(WiFi.localIP());

    // Connect to WebSocket server
    if (client.connect("192.168.1.21", 5000)) {
        Serial.println("Connected");
    } else {
        Serial.println("Connection failed.");
        while(1);  // Hang on failure
    }

    // Handshake with the WebSocket server
    webSocketClient.path = path;
    webSocketClient.host = host;
    if (webSocketClient.handshake(client)) {
        Serial.println("Handshake successful");
    } else {
        Serial.println("Handshake failed.");
        while(1);  // Hang on failure
    }
}

// Function to send data to all three shift registers
void writeToShiftRegister() {
    digitalWrite(LATCH_PIN, LOW);  // Prepare to shift data
    
    // Shift out data for each of the three shift registers (from MSB to LSB)
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, relayState[2]);  // Third shift register (relays 16-23)
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, relayState[1]);  // Second shift register
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, relayState[0]);  // First shift register

    digitalWrite(LATCH_PIN, HIGH);  // Latch the data to the output pins
}

// Function to send a ping message to keep the WebSocket connection alive
void sendPing() {
    if (client.connected()) {
        String pingMessage = "ping";
        webSocketClient.sendData(pingMessage);
        Serial.println("Ping sent to keep connection alive.");
    }
}

void loop() {
    String data;

    // Receive WebSocket data from HTML page
    if (webSocketClient.getData(data)) {
        if (data.length() > 0) {
            Serial.println("Received data: " + data);
            
            // Relay 1 ON/OFF (controlled by third shift register, pin 16)
            if (data == "relay1_on") {
                relayState[2] |= B00000001;  // Turn on relay 1 (pin 16)
                writeToShiftRegister();
                Serial.println("Relay 1 ON");
            } else if (data == "relay1_off") {
                relayState[2] &= ~B00000001;  // Turn off relay 1 (pin 16)
                writeToShiftRegister();
                Serial.println("Relay 1 OFF");
            }
            
            // Relay 2 ON/OFF (pin 17)
            if (data == "relay2_on") {
                relayState[2] |= B00000010;  // Turn on relay 2 (pin 17)
                writeToShiftRegister();
                Serial.println("Relay 2 ON");
            } else if (data == "relay2_off") {
                relayState[2] &= ~B00000010;  // Turn off relay 2 (pin 17)
                writeToShiftRegister();
                Serial.println("Relay 2 OFF");
            }

            // Relay 3 ON/OFF (pin 18)
            if (data == "relay3_on") {
                relayState[2] |= B00000100;  // Turn on relay 3 (pin 18)
                writeToShiftRegister();
                Serial.println("Relay 3 ON");
            } else if (data == "relay3_off") {
                relayState[2] &= ~B00000100;  // Turn off relay 3 (pin 18)
                writeToShiftRegister();
                Serial.println("Relay 3 OFF");
            }

            // Relay 4 ON/OFF (pin 19)
            if (data == "relay4_on") {
                relayState[2] |= B00001000;  // Turn on relay 4 (pin 19)
                writeToShiftRegister();
                Serial.println("Relay 4 ON");
            } else if (data == "relay4_off") {
                relayState[2] &= ~B00001000;  // Turn off relay 4 (pin 19)
                writeToShiftRegister();
                Serial.println("Relay 4 OFF");
            }

            // Repeat for relays 5-8 (pins 20-23) if needed
        }
    }

    // Physical Key Control for all 4 buttons

    // If Key 1 is pressed (transition from HIGH to LOW)
    buttonState1 = digitalRead(buttonPin1);
    if (buttonState1 == LOW && lastButtonState1 == HIGH) {
        relayState[2] ^= B00000001;  // Toggle relay 1 (pin 16)
        writeToShiftRegister();
        Serial.println("Key 1 pressed, relay 1 toggled.");
        webSocketClient.sendData("Key 1 pressed!");
    }

    // If Key 2 is pressed (transition from HIGH to LOW)
    buttonState2 = digitalRead(buttonPin2);
    if (buttonState2 == LOW && lastButtonState2 == HIGH) {
        relayState[2] ^= B00000010;  // Toggle relay 2 (pin 17)
        writeToShiftRegister();
        Serial.println("Key 2 pressed, relay 2 toggled.");
        webSocketClient.sendData("Key 2 pressed!");
    }

    // If Key 3 is pressed (transition from HIGH to LOW)
    buttonState3 = digitalRead(buttonPin3);
    if (buttonState3 == LOW && lastButtonState3 == HIGH) {
        relayState[2] ^= B00000100;  // Toggle relay 3 (pin 18)
        writeToShiftRegister();
        Serial.println("Key 3 pressed, relay 3 toggled.");
        webSocketClient.sendData("Key 3 pressed!");
    }

    // If Key 4 is pressed (transition from HIGH to LOW)
    buttonState4 = digitalRead(buttonPin4);
    if (buttonState4 == LOW && lastButtonState4 == HIGH) {
        relayState[2] ^= B00001000;  // Toggle relay 4 (pin 19)
        writeToShiftRegister();
        Serial.println("Key 4 pressed, relay 4 toggled.");
        webSocketClient.sendData("Key 4 pressed!");
    }

    // Update last button states
    lastButtonState1 = buttonState1;
    lastButtonState2 = buttonState2;
    lastButtonState3 = buttonState3;
    lastButtonState4 = buttonState4;

    // Send a ping message every 5 seconds to keep connection alive
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= pingInterval) {
        previousMillis = currentMillis;
        sendPing();
    }

    delay(300);  // Small delay to prevent overwhelming the loop
}

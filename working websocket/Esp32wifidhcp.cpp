#include <WiFi.h>
#include <WebSocketClient.h>

const char* ssid     = "Special Projects-5GHz";
const char* password = "sprojects1!";
char path[] = "/";
char host[] = "192.168.1.221:5000";

// WebSocket client and WiFi client
WebSocketClient webSocketClient;
WiFiClient client;

// Pins for buttons  (SN74HC595)
const int buttonPin1 = 18;  // GPIO for Key 1
const int buttonPin2 = 19;  // GPIO for Key 2
const int buttonPin3 = 21;  // GPIO for Key 3
const int buttonPin4 = 23;  // GPIO for Key 4

//pins for the SN74HC595 shift register
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
const long pingInterval = 100; // Send ping every 100 ms

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
    if (client.connect("192.168.1.221", 5000)) {
        Serial.println("Connected");
    } else {
        Serial.println("Connection failed.");
        connectToWebSocket();  // attempt reconnect
    }

    // Handshake with the WebSocket server
    webSocketClient.path = path;
    webSocketClient.host = host;
    if (webSocketClient.handshake(client)) {
        Serial.println("Handshake successful");
    } else {
        Serial.println("Handshake failed.");
        connectToWebSocket();  // attempt reconnect
    }
}
void connectToWebSocket() {
    // Try to connect to the WebSocket server
    while (!client.connect("192.168.1.221", 5000)) {
        Serial.println("Connection failed. Retrying in 5 seconds...");
        delay(5000);  // Wait 5 seconds before retrying
    }
    Serial.println("Connected");

    // Try WebSocket handshake
    webSocketClient.path = path;
    webSocketClient.host = host;
    
    while (!webSocketClient.handshake(client)) {
        Serial.println("Handshake failed. Retrying in 1 second...");
        delay(1000);  // Wait 5 seconds before retrying handshake
    }
    
    Serial.println("Handshake successful");
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

// Function to send a ping message to keep the WebSocket connection alive and transmit state
void sendButtonStates() {
    // Read the current state of all buttons
    int button1State = digitalRead(buttonPin1) == LOW ? 1 : 0;
    int button2State = digitalRead(buttonPin2) == LOW ? 1 : 0;
    int button3State = digitalRead(buttonPin3) == LOW ? 1 : 0;
    int button4State = digitalRead(buttonPin4) == LOW ? 1 : 0;

    // Format the message as "1,0,0,1" for the button states
    String buttonStates = String(button1State) + "," + 
                          String(button2State) + "," + 
                          String(button3State) + "," + 
                          String(button4State);

    // Send the button states via WebSocket if connected
    if (client.connected()) {
        webSocketClient.sendData(buttonStates);
        Serial.println("Button states sent: " + buttonStates);
    }
}


void loop() {
    String data;


 if (!client.connected()) {
        Serial.println("Disconnected from WebSocket, attempting to reconnect...");
        connectToWebSocket();
    }
    // Receive WebSocket data from HTML page
if (webSocketClient.getData(data)) {
    if (data.length() > 0) {
        Serial.println("Received data: " + data);
        // Relay 1 ON/OFF (controlled by third shift register, pin 16)
        //    if (data == "relay1_on") {
        //        relayState[2] |= B00000001;  // Turn on relay 1 (pin 16)
        //        writeToShiftRegister();
        //        Serial.println("Relay 1 ON");
        //    } else if (data == "relay1_off") {
        //        relayState[2] &= ~B00000001;  // Turn off relay 1 (pin 16)
        //        writeToShiftRegister();
        //      Serial.println("Relay 1 OFF");
        //    }consolodated looping thru the shift regester into one funtion
        // Loop through the relays from 1 to 8
        for (int i = 1; i <= 8; i++) {
            String relayOnCmd = "relay" + String(i) + "_on";
            String relayOffCmd = "relay" + String(i) + "_off";
            
            if (data == relayOnCmd) {
                relayState[2] |= (1 << (i - 1));  // Turn on relay (shift corresponding bit)
                writeToShiftRegister();
                Serial.println("Relay " + String(i) + " ON");
            } else if (data == relayOffCmd) {
                relayState[2] &= ~(1 << (i - 1));  // Turn off relay (clear corresponding bit)
                writeToShiftRegister();
                Serial.println("Relay " + String(i) + " OFF");
            }
        }
    }
}

    // Physical Key Control for all 4 buttons

    // If Key 1 is pressed (transition from HIGH to LOW)
    //buttonState1 = digitalRead(buttonPin1);
    //if (buttonState1 == LOW && lastButtonState1 == HIGH) {
    //    relayState[2] ^= B00000001;  // Toggle relay 1 (pin 16)
    //    writeToShiftRegister();
    //    Serial.println("Key 1 pressed, relay 1 toggled.");
    //     webSocketClient.sendData("Key 1 pressed!");
    //}consolodated the 4 buttons in to one funtion

    for (int i = 0; i < 4; i++) {
    // Get correct button pin for each iteration
    int buttonPin = (i == 0) ? buttonPin1 : (i == 1) ? buttonPin2 : (i == 2) ? buttonPin3 : buttonPin4;
    
    // Read current button state
    int buttonState = digitalRead(buttonPin);
    
    // Get correct last button state
    int lastButtonState = (i == 0) ? lastButtonState1 : (i == 1) ? lastButtonState2 : (i == 2) ? lastButtonState3 : lastButtonState4;

    // Check if button is pressed (transition from HIGH to LOW)
    if (buttonState == LOW && lastButtonState == HIGH) {
        relayState[2] ^= (1 << i);  // Toggle the correct relay
        writeToShiftRegister();
        Serial.println("Key " + String(i + 1) + " pressed, relay " + String(i + 1) + " toggled.");
        webSocketClient.sendData("Key " + String(i + 1) + " pressed!");
    }

    // Update the correct last button state
    if (i == 0) lastButtonState1 = buttonState;
    else if (i == 1) lastButtonState2 = buttonState;
    else if (i == 2) lastButtonState3 = buttonState;
    else lastButtonState4 = buttonState;
}
    // Send a ping message every 5 seconds to keep connection alive
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= pingInterval) {
        previousMillis = currentMillis;
      sendButtonStates();

    }

    delay(100);  // Small delay to prevent overwhelming the loop
}

#include <WiFi.h>
#include <SocketIoClient.h>  // Include the Socket.IO client header

const char* ssid     = "Special Projects-5GHz";
const char* password = "sprojects1!";
char host[] = "192.168.1.221";
int port = 5000;

SocketIoClient socketIoClient;

// Pins for buttons (SN74HC595)  
const int buttonPin1 = 18;
const int buttonPin2 = 19;
const int buttonPin3 = 21;
const int buttonPin4 = 23;

//pins for the SN74HC595 shift register
#define DATA_PIN   13
#define CLOCK_PIN  27
#define LATCH_PIN  14
#define OE_PIN     4

byte relayState[3] = { B00000000, B00000000, B00000000 };

int buttonState1 = HIGH, buttonState2 = HIGH, buttonState3 = HIGH, buttonState4 = HIGH;
int lastButtonState1 = HIGH, lastButtonState2 = HIGH, lastButtonState3 = HIGH, lastButtonState4 = HIGH;

unsigned long previousMillis = 0;
const long pingInterval = 100;

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
    
    digitalWrite(OE_PIN, LOW);
    digitalWrite(LATCH_PIN, LOW);

    // Connect to WiFi
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi.");

    // Initialize Socket.IO connection
    socketIoClient.begin(host, port);
    socketIoClient.on("relay_control", relayControlEvent);  // Event listener for relay control
}

void relayControlEvent(const char * payload, size_t length) {
    String data = String(payload);

    // Loop through the relays from 1 to 8
    for (int i = 1; i <= 8; i++) {
        String relayOnCmd = "relay" + String(i) + "_on";
        String relayOffCmd = "relay" + String(i) + "_off";
        
        if (data == relayOnCmd) {
            relayState[2] |= (1 << (i - 1));  // Turn on relay
            writeToShiftRegister();
            Serial.println("Relay " + String(i) + " ON");
        } else if (data == relayOffCmd) {
            relayState[2] &= ~(1 << (i - 1));  // Turn off relay
            writeToShiftRegister();
            Serial.println("Relay " + String(i) + " OFF");
        }
    }
}

void writeToShiftRegister() {
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, relayState[2]);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, relayState[1]);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, relayState[0]);
    digitalWrite(LATCH_PIN, HIGH);
}

void sendButtonStates() {
    int button1State = digitalRead(buttonPin1) == LOW ? 1 : 0;
    int button2State = digitalRead(buttonPin2) == LOW ? 1 : 0;
    int button3State = digitalRead(buttonPin3) == LOW ? 1 : 0;
    int button4State = digitalRead(buttonPin4) == LOW ? 1 : 0;

    String buttonStates = String(button1State) + "," + 
                          String(button2State) + "," + 
                          String(button3State) + "," + 
                          String(button4State);

    socketIoClient.emit("button_states", buttonStates.c_str());
    Serial.println("Button states sent: " + buttonStates);
}

void loop() {
    // Maintain the Socket.IO connection
    socketIoClient.loop();

    for (int i = 0; i < 4; i++) {
        int buttonPin = (i == 0) ? buttonPin1 : (i == 1) ? buttonPin2 : (i == 2) ? buttonPin3 : buttonPin4;
        int buttonState = digitalRead(buttonPin);
        int lastButtonState = (i == 0) ? lastButtonState1 : (i == 1) ? lastButtonState2 : (i == 2) ? lastButtonState3 : lastButtonState4;

        if (buttonState == LOW && lastButtonState == HIGH) {
            relayState[2] ^= (1 << i);
            writeToShiftRegister();
            Serial.println("Key " + String(i + 1) + " pressed, relay " + String(i + 1) + " toggled.");
            socketIoClient.emit("button_pressed", String("Key " + String(i + 1) + " pressed!").c_str());
        }

        if (i == 0) lastButtonState1 = buttonState;
        else if (i == 1) lastButtonState2 = buttonState;
        else if (i == 2) lastButtonState3 = buttonState;
        else lastButtonState4 = buttonState;
    }

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= pingInterval) {
        previousMillis = currentMillis;
        sendButtonStates();
    }
}

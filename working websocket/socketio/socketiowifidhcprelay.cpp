#include <WiFi.h>
#include <SocketIoClient.h>
#include <ESPAsyncWebServer.h>
#include <Update.h>  // For OTA updates

const char* ssid     = "Special Projects-5GHz";
const char* password = "sprojects1!";
char host[] = "192.168.1.221";
int port = 5000;

SocketIoClient socketIoClient;
AsyncWebServer server(80);  // Create AsyncWebServer object on port 80

// Pins for buttons (SN74HC595)
const int buttonPin1 = 18;
const int buttonPin2 = 19;
const int buttonPin3 = 21;
const int buttonPin4 = 23;

// Pins for the SN74HC595 shift register
#define DATA_PIN   13
#define CLOCK_PIN  27
#define LATCH_PIN  14
#define OE_PIN     4

byte relayState[3] = { B00000000, B00000000, B00000000 };

int buttonState1 = HIGH, buttonState2 = HIGH, buttonState3 = HIGH, buttonState4 = HIGH;
int lastButtonState1 = HIGH, lastButtonState2 = HIGH, lastButtonState3 = HIGH, lastButtonState4 = HIGH;

unsigned long previousMillis = 0;
const long pingInterval = 2000;

// Function to write data to the shift register
void writeToShiftRegister() {
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, relayState[2]);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, relayState[1]);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, relayState[0]);
    digitalWrite(LATCH_PIN, HIGH);
}

// Event handler for relay control from the server
void relayControlEvent(const char * payload, size_t length) {
    String data = String(payload);
    Serial.println("Relay control command received: " + data);

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

void sendButtonStates() {
    int button1State = digitalRead(buttonPin1) == LOW ? 1 : 0;
    int button2State = digitalRead(buttonPin2) == LOW ? 1 : 0;
    int button3State = digitalRead(buttonPin3) == LOW ? 1 : 0;
    int button4State = digitalRead(buttonPin4) == LOW ? 1 : 0;

    String buttonStates = "{\"Button1\": " + String(button1State) +
                         ", \"Button2\": " + String(button2State) +
                         ", \"Button3\": " + String(button3State) +
                         ", \"Button4\": " + String(button4State) + "}";

    socketIoClient.emit("button_states", buttonStates.c_str());
    Serial.println("Button states sent: " + buttonStates);
}

// Function to create a JSON string of the button states
String getButtonStatesJSON() {
    int button1State = digitalRead(buttonPin1) == LOW ? 1 : 0;
    int button2State = digitalRead(buttonPin2) == LOW ? 1 : 0;
    int button3State = digitalRead(buttonPin3) == LOW ? 1 : 0;
    int button4State = digitalRead(buttonPin4) == LOW ? 1 : 0;

    String buttonStates = "{\"button1\": " + String(button1State) +
                         ", \"button2\": " + String(button2State) +
                         ", \"button3\": " + String(button3State) +
                         ", \"button4\": " + String(button4State) + "}";
    return buttonStates;
}
// Handle the file upload for OTA updates
void handleOTAUpdate(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
        Serial.printf("UploadStart: %s\n", filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {  // Begin OTA update
            Update.printError(Serial);
        }
    }

    // Write the data to the flash memory
    if (Update.write(data, len) != len) {
        Update.printError(Serial);
    }

    if (final) {  // Finish the update
        if (Update.end(true)) {
            Serial.printf("UpdateSuccess: %s\n", filename.c_str());
        } else {
            Update.printError(Serial);
        }
    }
}

// Function to serve the webpage that shows button states and provides file upload for OTA
void serveWebPage() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        String htmlPage = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
        htmlPage += "<title>ESP32 Button States</title></head><body>";
        htmlPage += "<h1>Button States</h1><div id='buttonStates'>Loading...</div>";
        htmlPage += "<script>setInterval(function() {fetch('/button_states').then(response => response.json()).then(data => {";
        htmlPage += "document.getElementById('buttonStates').innerHTML = 'Button 1: ' + data.button1 + '<br>Button 2: ' + data.button2 + '<br>Button 3: ' + data.button3 + '<br>Button 4: ' + data.button4;";
        htmlPage += "});}, 2000);</script>";

        // Add a form for uploading firmware
        htmlPage += "<h2>Upload Firmware (.bin)</h2><form method='POST' action='/update' enctype='multipart/form-data'>";
        htmlPage += "<input type='file' name='update'><input type='submit' value='Update Firmware'></form>";
        htmlPage += "</body></html>";
        request->send(200, "text/html", htmlPage);
    });

    // Serve the button states as JSON data
    server.on("/button_states", HTTP_GET, [](AsyncWebServerRequest *request){
        String json = getButtonStatesJSON();
        request->send(200, "application/json", json);
    });
    // Handle firmware update via file upload
    server.onFileUpload(handleOTAUpdate);
    server.begin();  // Start the server
}

// Setup function
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
    Serial.print("Assigned IP address: ");
    Serial.println(WiFi.localIP());
    // Initialize Socket.IO connection
    socketIoClient.begin(host, port);
    socketIoClient.on("relay_control", relayControlEvent);  // Event listener for relay control

    // Start web server to serve button states page
    serveWebPage();
}

// Loop function
void loop() {
    // Maintain the Socket.IO connection
    socketIoClient.loop();

    // Check button presses and toggle relays
    for (int i = 0; i < 4; i++) {
        int buttonPin = (i == 0) ? buttonPin1 : (i == 1) ? buttonPin2 : (i == 2) ? buttonPin3 : buttonPin4;
        int buttonState = digitalRead(buttonPin);
        int &lastButtonState = (i == 0) ? lastButtonState1 : (i == 1) ? lastButtonState2 : (i == 2) ? lastButtonState3 : lastButtonState4;

        if (buttonState == LOW && lastButtonState == HIGH) {
            relayState[2] ^= (1 << i);  // Toggle relay
            writeToShiftRegister();
            Serial.println("Key " + String(i + 1) + " pressed, relay " + String(i + 1) + " toggled.");
    
            // Log and send message
            String message = "{\"key\": " + String(i + 1) + "}";
            Serial.println("Sending message: " + message);
            socketIoClient.emit("button_pressed", message.c_str());
        }

        // Update lastButtonState after checking
        lastButtonState = buttonState;
    }

    // Send button states periodically
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= pingInterval) {
        previousMillis = currentMillis;
        sendButtonStates();
    }
}

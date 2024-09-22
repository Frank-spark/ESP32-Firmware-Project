#include <WiFi.h>
#include <WebSocketClient.h>

const char* ssid     = "Special Projects-5GHz";
const char* password = "sprojects1!";
char path[] = "/";
char host[] = "192.168.1.221:5000";

WebSocketClient webSocketClient;
WiFiClient client;

// Define the button pins
const int buttonPin1 = 18;  // GPIO for Key 1
const int buttonPin2 = 19;  // GPIO for Key 2
const int buttonPin3 = 21;  // GPIO for Key 3
const int buttonPin4 = 23;  // GPIO for Key 4

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

  // Initialize the button pins as inputs with internal pull-up resistors
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buttonPin4, INPUT_PULLUP);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(5000);

  // Connect to the websocket server
  if (client.connect("192.168.1.221", 5000)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
    while(1) {
      // Hang on failure
    }
  }

  // Handshake with the server
  webSocketClient.path = path;
  webSocketClient.host = host;
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");
    while(1) {
      // Hang on failure
    }  
  }
}

// Function to send a ping message to the server
void sendPing() {
  if (client.connected()) {
    String pingMessage = "ping";
    webSocketClient.sendData(pingMessage);
    Serial.println("Ping sent to keep connection alive.");
  }
}

void loop() {
  String data;

  // Check if the client is connected
  if (client.connected()) {
    // Check each button state
    buttonState1 = digitalRead(buttonPin1);
    buttonState2 = digitalRead(buttonPin2);
    buttonState3 = digitalRead(buttonPin3);
    buttonState4 = digitalRead(buttonPin4);

    // If Key 1 is pressed (transition from HIGH to LOW)
    if (buttonState1 == LOW && lastButtonState1 == HIGH) {
      data = "Key 1 pressed!";
      webSocketClient.sendData(data);
      Serial.println("Key 1 pressed, message sent.");
    }

    // If Key 2 is pressed (transition from HIGH to LOW)
    if (buttonState2 == LOW && lastButtonState2 == HIGH) {
      data = "Key 2 pressed!";
      webSocketClient.sendData(data);
      Serial.println("Key 2 pressed, message sent.");
    }

    // If Key 3 is pressed (transition from HIGH to LOW)
    if (buttonState3 == LOW && lastButtonState3 == HIGH) {
      data = "Key 3 pressed!";
      webSocketClient.sendData(data);
      Serial.println("Key 3 pressed, message sent.");
    }

    // If Key 4 is pressed (transition from HIGH to LOW)
    if (buttonState4 == LOW && lastButtonState4 == HIGH) {
      data = "Key 4 pressed!";
      webSocketClient.sendData(data);
      Serial.println("Key 4 pressed, message sent.");
    }

    // Update last button states for the next loop iteration
    lastButtonState1 = buttonState1;
    lastButtonState2 = buttonState2;
    lastButtonState3 = buttonState3;
    lastButtonState4 = buttonState4;

    // Send ping message every 5 seconds to keep connection alive
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= pingInterval) {
      previousMillis = currentMillis;
      sendPing();
    }

  } else {
    Serial.println("Client disconnected. Attempting to reconnect...");
    
    // Attempt to reconnect
    if (client.connect("192.168.1.221", 5000)) {
      webSocketClient.handshake(client);
      Serial.println("Reconnected successfully.");
    } else {
      Serial.println("Reconnection failed.");
      delay(1000); // Wait 1 second before retrying
    }
  }

  // Short delay to prevent overwhelming the loop
  delay(300);
}

#include <Arduino.h>
#include <ETH.h>
#include <SocketIOclient.h>
#include <Update.h>
#include <HTTPClient.h>

// Network configuration
IPAddress local_IP(192, 168, 8, 190);   // Static IP for ESP32 client
IPAddress gateway(192, 168, 8, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

// SocketIO client instance
SocketIOclient socketIO;

void setup() {
  Serial.begin(115200);

  WiFi.disconnect(true);  // Disable WiFi
  WiFi.mode(WIFI_OFF);    // Turn WiFi off
  btStop();               // Stop Bluetooth

  // Initialize Ethernet
  if (!ETH.begin(1, 16, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_IN)) {
    Serial.println("Failed to initialize Ethernet");
    return;
  }

  // Configure static IP
  if (!ETH.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Static IP configuration failed");
  }

  Serial.println("Starting Ethernet...");
  while (!ETH.linkUp()) {
    delay(1000);
    Serial.println("Waiting for Ethernet...");
  }

  Serial.print("Connected to Ethernet. IP address: ");
  Serial.println(ETH.localIP());

  // Connect to the JavaScript server using Socket.IO
  socketIO.begin("192.168.8.214", 3000);  // Replace with the JS server's IP and port
  socketIO.onEvent(socketIOEvent);        // Attach the event handler
}

void loop() {
  socketIO.loop();  // Continuously check for incoming WebSocket messages
  delay(1);
}

// Event handler for Socket.IO events
void socketIOEvent(socketIOmessageType_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case sIOtype_CONNECT:
      Serial.println("[SocketIO] Connected to server");
      break;
    case sIOtype_DISCONNECT:
      Serial.println("[SocketIO] Disconnected from server");
      break;
    case sIOtype_EVENT:
      Serial.printf("[SocketIO] Received event: %s\n", payload);
      break;
    default:
      Serial.printf("[SocketIO] Unknown event: %d\n", type);
      break;
  }
}

// Firmware update handling function
void checkForFirmwareUpdate() {
  HTTPClient http;
  http.begin("http://192.168.1.155:3000/firmware.bin"); // Replace with JS server firmware URL
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    int len = http.getSize();
    uint8_t buff[128] = { 0 };
    WiFiClient* stream = http.getStreamPtr();
    if (!Update.begin(len)) {
      Update.printError(Serial);
    }
    while (http.connected() && len > 0) {
      int c = stream->readBytes(buff, ((len > sizeof(buff)) ? sizeof(buff) : len));
      Update.write(buff, c);
      len -= c;
    }
    if (Update.end(true)) {
      Serial.println("Update success, rebooting...");
      ESP.restart();
    } else {
      Update.printError(Serial);
    }
  }
  http.end();
}

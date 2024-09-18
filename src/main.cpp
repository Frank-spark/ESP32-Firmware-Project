#include <Arduino.h>
#include <ETH.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <WebSockets.h>
#include <WebServer_WT32_ETH01.h> 
#include <esp_spiffs.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>
#include "index.h"

// ----------------libraries already included will be shortened and ones bellow may be added as additional features are implemented-------------------

// #include <EthernetEspAT.h>
// #include <WiFiEspAT.h>
// #include <SocketIoClient.h>  
// #include <esp_wifi.h>
// #include <ArduinoWebsockets.h>
// #include <AsyncWebSocket.h>
// #include <ESPAsyncWebServer.h>

// ----------------------network credentials-------------------------------
const char* host = "esp32";
IPAddress local_IP(192, 168, 1, 190);  //change to match within your network range//
IPAddress gateway(192, 168, 1, 254);    //will be the same as your router// 
IPAddress subnet(255, 255, 255, 0);   
IPAddress primaryDNS(8, 8, 8, 8);     
IPAddress secondaryDNS(8, 8, 4, 4);   

// AsyncWebServer server(80);
// AsyncWebSocket ws("/ws");
WebServer server(80);
// SocketIOclient socketIO;

void setup() {
  Serial.begin(115200);

  WiFi.disconnect(true);  // Disable WiFi
  WiFi.mode(WIFI_OFF);
  btStop();

  // Initialize Ethernet
  if (!ETH.begin(1, 16, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_IN)) {
    Serial.println("Failed to initialize Ethernet");
    return;
  }

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

  // socketIO.begin("192, 168, 1, 155", 80);  // Replace with your Socket.IO server address
  // socketIO.onEvent(socketIOEvent);  // Listen for the "connect" event

   if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

   /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
}

void loop(void) {
  server.handleClient();
  delay(1);
}

#include "WiFiSetup.h"
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <Update.h>

WebServer otaServer(8080); // Instantiate otaServer on port 8080

void setupWiFiAndWebServer() {
    Serial.println("Initializing WiFi Manager...");
    WiFiManager wm; // Create a WiFiManager instance

    // Automatically try to connect to the last saved network
    // If it fails, it starts an access point with a captive portal
    if (!wm.autoConnect("ESP32-Setup", "password123")) { // Access point SSID and password
        Serial.println("Failed to connect to WiFi, restarting...");
        delay(3000);
        ESP.restart();
    }
    Serial.println("WiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // OTA setup
    ArduinoOTA.onStart([]() {
        String type = ArduinoOTA.getCommand() == U_FLASH ? "sketch" : "filesystem";
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
    ArduinoOTA.begin();
    Serial.println("OTA ready and hosted on port 8080.");

    // Handle firmware upload form
    otaServer.on("/", HTTP_GET, []() {
        otaServer.send(200, "text/html", "<html><body><h1>Upload Firmware</h1>"
                                         "<form method='POST' action='/update' enctype='multipart/form-data'>"
                                         "<input type='file' name='update'><br>"
                                         "<input type='submit' value='Update'></form></body></html>");
    });

    otaServer.on("/update", HTTP_POST, []() {
        otaServer.send(200, "text/plain", Update.hasError() ? "Update Failed!" : "Update Successful! Rebooting...");
        delay(100);
        ESP.restart();
    }, []() {
        HTTPUpload& upload = otaServer.upload();
        if (upload.status == UPLOAD_FILE_START) {
            Serial.printf("Update Start: %s\n", upload.filename.c_str());
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
            Serial.printf("Updating: %u bytes\n", upload.currentSize);
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_END) {
            if (Update.end(true)) {
                Serial.printf("Update Success: %u bytes\n", upload.totalSize);
            } else {
                Update.printError(Serial);
            }
        }
    });

    otaServer.begin(); // Start OTA server
}

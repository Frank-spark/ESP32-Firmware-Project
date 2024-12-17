#include "WiFiSetup.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <Update.h>

WebServer otaServer(8080); // Instantiate otaServer on port 8080

const char* ssid = "Special Projects-5GHz";
const char* password = "sprojects1!";

void setupWiFiAndWebServer() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
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



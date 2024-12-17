#include "OTAsetup.h"

// Web server instance on port 8080
WebServer server(8080);

const char* OTAusername = "admin";
const char* OTApassword = "admin";

// HTML login and firmware upload page
const char* uploadPage PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 OTA Update</title>
</head>
<body style="font-family: Arial; text-align: center;">
    <h2>ESP32 OTA Update</h2>
    <form method="POST" action="/update" enctype="multipart/form-data">
        <input type="file" name="firmware" accept=".bin" required>
        <br><br>
        <input type="submit" value="Upload Firmware">
    </form>
</body>
</html>
)rawliteral";

// Function to serve the login page
void handleLogin() {
    if (!server.authenticate(OTAusername, OTApassword)) {
        return server.requestAuthentication();
    }
    server.send(200, "text/html", uploadPage);
}

// Function to handle firmware upload
void handleFirmwareUpdate() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { // Start the update
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { // End the update
            Serial.println("Update successful");
        } else {
            Update.printError(Serial);
        }
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", "Update Finished. Rebooting...");
        delay(1000);
        ESP.restart();
    }
}

void setupOTA() {
    // Setup HTTP basic authentication
    server.on("/", HTTP_GET, []() {
        if (!server.authenticate(OTAusername, OTApassword)) {
            return server.requestAuthentication();
        }
        server.send(200, "text/html", uploadPage);
    });

    // Handle firmware upload
    server.on("/update", HTTP_POST, []() {
        if (!server.authenticate(OTAusername, OTApassword)) {
            return server.requestAuthentication();
        }
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", Update.hasError() ? "Update Failed!" : "Update Successful! Rebooting...");
        delay(100);
        ESP.restart();
    }, []() {
        // Handle the file upload
        HTTPUpload& upload = server.upload();
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
    });  // Missing closing parenthesis here

    server.begin();
}

void handleOTA() {
    ArduinoOTA.handle(); // Handle OTA updates
    server.handleClient(); // Handle HTTP requests
}

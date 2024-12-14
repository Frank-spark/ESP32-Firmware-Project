#include <WiFi.h>
#include <WebServer.h>
#include <Update.h> // Include for OTA functionality

// Define the web server on port 8080
WebServer server(8080);

// Hardcoded WiFi credentials
const char* ssid = "Special Projects-5GHz";
const char* password = "sprojects1!";

void setupWiFiAndWebServer() {
    Serial.begin(115200);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Serve the main HTML page directly from the code
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", R"rawliteral(
            <html>
                <body>
                    <h1>ESP32 Web Server</h1>
                    <p>Welcome to the ESP32 Web Server!</p>
                    <form method="POST" action="/upload" enctype="multipart/form-data">
                        <label for="file">Upload .bin File for OTA Update:</label><br>
                        <input type="file" name="file"><br><br>
                        <input type="submit" value="Upload">
                    </form>
                </body>
            </html>
        )rawliteral");
    });

    // Handle OTA file upload
    server.on("/upload", HTTP_POST, []() {
        server.send(200, "text/plain", "OTA Update Complete. Please reboot your ESP32.");
    }, []() {
        HTTPUpload& upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
            Serial.printf("OTA Update Started: %s\n", upload.filename.c_str());
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { // Start OTA update process
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_END) {
            if (Update.end(true)) { // Finalize OTA process
                Serial.printf("OTA Update Successful: %u bytes\n", upload.totalSize);
            } else {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_ABORTED) {
            Update.end();
            Serial.println("OTA Update Aborted");
        }
    });

    // Start the server
    server.begin();
    Serial.println("Web server started on port 8080");
}

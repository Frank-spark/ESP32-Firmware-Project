#include <Adafruit_NeoPixel.h>
#include "WiFiSetup.h"
#include "OTASetup.h"
#include "SocketIOSetup.h"
#include <WebServer.h>

#define PIN        21
#define NUMPIXELS  12
#define ACTIVE_LEDS 7  // Number of active LEDs (1 through 7)

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Declare float adjustment server on port 3000
WebServer floatServer(4000);

// Global variables
int floatLevel = 0;
int brightness = 255;  // Default brightness (max 255)
unsigned long lastUpdate = 0; // For non-blocking LED updates
bool toggleState = false;     // For flashing LEDs

// Function to set LED brightness
uint32_t adjustBrightness(uint32_t color, int brightness) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    r = (r * brightness) / 255;
    g = (g * brightness) / 255;
    b = (b * brightness) / 255;

    return pixels.Color(r, g, b);
}

// Update LEDs based on float level and brightness
void updateLEDs() {
    static int lastFloatLevel = -1; // Store the previous level to minimize updates
    if (floatLevel == lastFloatLevel && millis() - lastUpdate < 100) {
        return; // Skip update if nothing has changed or it's too soon
    }
    lastUpdate = millis();
    lastFloatLevel = floatLevel;

    // Default all active LEDs (1 through 7) to glowing white
    for (int i = 0; i < ACTIVE_LEDS; i++) {
        pixels.setPixelColor(i, adjustBrightness(pixels.Color(50, 50, 50), brightness)); // Soft white glow
    }

    if (floatLevel < 10) {
        // First LED flashing red
        pixels.setPixelColor(0, toggleState ? adjustBrightness(pixels.Color(255, 0, 0), brightness)
                                            : adjustBrightness(pixels.Color(50, 50, 50), brightness));
    } else if (floatLevel < 20) {
        pixels.setPixelColor(0, adjustBrightness(pixels.Color(255, 255, 0), brightness)); // Yellow
    } else if (floatLevel < 30) {
        pixels.setPixelColor(0, adjustBrightness(pixels.Color(255, 255, 0), brightness));
        pixels.setPixelColor(1, adjustBrightness(pixels.Color(255, 255, 0), brightness));
    } else if (floatLevel < 40) {
        for (int i = 0; i < 3; i++) {
            pixels.setPixelColor(i, adjustBrightness(pixels.Color(0, 255, 0), brightness)); // Green
        }
    } else if (floatLevel < 50) {
        for (int i = 0; i < 4; i++) {
            pixels.setPixelColor(i, adjustBrightness(pixels.Color(0, 255, 0), brightness));
        }
    } else if (floatLevel < 60) {
        for (int i = 0; i < 5; i++) {
            pixels.setPixelColor(i, adjustBrightness(pixels.Color(0, 0, 255), brightness)); // Blue
        }
    } else if (floatLevel < 70) {
        for (int i = 0; i < 6; i++) {
            pixels.setPixelColor(i, adjustBrightness(pixels.Color(0, 0, 255), brightness));
        }
    } else {
        for (int i = 0; i < ACTIVE_LEDS; i++) {
            pixels.setPixelColor(i, toggleState ? adjustBrightness(pixels.Color(255, 0, 0), brightness)
                                                : adjustBrightness(pixels.Color(50, 50, 50), brightness)); // Flash red
        }
    }

    toggleState = !toggleState; // Toggle the state for flashing LEDs

    // Turn off LEDs 8 to 12
    for (int i = ACTIVE_LEDS; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }

    pixels.show();
}

// HTML handler for float adjustment
void handleFloatPage() {
    floatServer.send(200, "text/html", R"rawliteral(
        <!DOCTYPE html>
        <html>
        <head>
            <title>Float and Brightness Control</title>
            <style>
                body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
                input[type="range"] { width: 80%; margin: 10px; }
            </style>
        </head>
        <body>
            <h1>Float and Brightness Control</h1>
            <div>
                <p>Adjust Float Level:</p>
                <input type="range" id="floatSlider" min="0" max="70" value="0" oninput="updateFloat(this.value)">
                <p>Current Float Level: <span id="floatValue">0</span></p>
            </div>
            <div>
                <p>Adjust Brightness:</p>
                <input type="range" id="brightnessSlider" min="0" max="255" value="255" oninput="updateBrightness(this.value)">
                <p>Current Brightness: <span id="brightnessValue">255</span></p>
            </div>

            <script>
                function updateFloat(value) {
                    document.getElementById('floatValue').textContent = value;
                    fetch(`/setLevel?value=${value}`);
                }
                function updateBrightness(value) {
                    document.getElementById('brightnessValue').textContent = value;
                    fetch(`/setBrightness?value=${value}`);
                }
            </script>
        </body>
        </html>
    )rawliteral");
}

// Slider update handler for float level
void handleSetFloatLevel() {
    if (floatServer.hasArg("value")) {
        floatLevel = floatServer.arg("value").toInt();
        floatServer.send(200, "text/plain", "OK");
    } else {
        floatServer.send(400, "text/plain", "Missing 'value' parameter");
    }
}

// Slider update handler for brightness
void handleSetBrightness() {
    if (floatServer.hasArg("value")) {
        brightness = floatServer.arg("value").toInt();
        floatServer.send(200, "text/plain", "OK");
    } else {
        floatServer.send(400, "text/plain", "Missing 'value' parameter");
    }
}

void setup() {
    Serial.begin(115200);         // Initialize serial communication
    setupWiFiAndWebServer();      // Set up WiFi and Web Server
    setupOTA();                   // Set up OTA
    setupSocketIO();              // Initialize Socket.IO client

    pixels.begin();               // Initialize NeoPixel
    pixels.show();                // Ensure all LEDs are off initially

    // Define routes for float server
    floatServer.on("/index", handleFloatPage);          // Serve HTML page
    floatServer.on("/setLevel", handleSetFloatLevel);   // Handle float slider changes
    floatServer.on("/setBrightness", handleSetBrightness); // Handle brightness slider
    floatServer.begin();          // Start the float server

    // Debugging information
    Serial.println("System initialized.");
    Serial.println("Access OTA at:");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println(":8080/");

    Serial.println("Access Float and Brightness Control at:");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println(":000/index");
}

void loop() {
    otaServer.handleClient();    // Handle OTA requests
    floatServer.handleClient();  // Handle float adjustment requests
    updateLEDs();    
    handleSocketIO();        // Handle Socket.IO communication
    delay(10);               // Small delay for stability            // Non-blocking LED updates
}

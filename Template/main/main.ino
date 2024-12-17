#include <Adafruit_NeoPixel.h>
#include "WiFiSetup.h"
#include "OTASetup.h"
#include "SocketIOSetup.h"
#include <WebServer.h>


#define PIN        21
#define NUMPIXELS  12
#define ACTIVE_LEDS 7  // Number of active LEDs (1 through 7)

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int brightness = 50;
int floatLevel = 50;

// Global variables

unsigned long lastUpdate = 0; // For non-blocking LED updates
bool toggleState = false;     // For flashing LEDs
int flashRate = 500;          // Flashing rate in milliseconds

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

void updateLEDs() {
    static int lastFloatLevel = -1; // Store the previous level to minimize updates
    static unsigned long lastFlashToggle = 0; // Track last toggle for flashing

    if (floatLevel == lastFloatLevel && millis() - lastUpdate < 100) {
        return; // Skip update if nothing has changed or it’s too soon
    }

    lastUpdate = millis();
    lastFloatLevel = floatLevel;

    // Handle flashing toggle based on flash rate
    if (millis() - lastFlashToggle >= flashRate) {
        toggleState = !toggleState;
        lastFlashToggle = millis();
    }

    // Reset all LEDs
    for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, adjustBrightness(pixels.Color(0, 0, 0), brightness));
    }

    // Handle LED states based on floatLevel
    if (floatLevel < 5) { // All LEDs flash red for very low level
        for (int i = 0; i < NUMPIXELS; i++) {
            pixels.setPixelColor(i, toggleState ? adjustBrightness(pixels.Color(255, 0, 0), brightness)
                                                : adjustBrightness(pixels.Color(0, 0, 0), brightness));
        }
    } else if (floatLevel <= 10) { // First two LEDs pulse white
        pixels.setPixelColor(0, toggleState ? adjustBrightness(pixels.Color(255, 255, 255), brightness)
                                            : adjustBrightness(pixels.Color(50, 50, 50), brightness));
        pixels.setPixelColor(1, toggleState ? adjustBrightness(pixels.Color(255, 255, 255), brightness)
                                            : adjustBrightness(pixels.Color(50, 50, 50), brightness));
    } else if (floatLevel <= 25) { // Near-empty state
        pixels.setPixelColor(0, adjustBrightness(pixels.Color(255, 255, 255), brightness));
        pixels.setPixelColor(1, toggleState ? adjustBrightness(pixels.Color(255, 255, 255), brightness)
                                            : adjustBrightness(pixels.Color(50, 50, 50), brightness));
    } else if (floatLevel < 90) { // Filling state
        int numLit = map(floatLevel, 0, 100, 0, ACTIVE_LEDS);
        for (int i = 0; i < numLit; i++) {
            pixels.setPixelColor(i, adjustBrightness(pixels.Color(0, 255, 0), brightness));
        }
        if (numLit >= ACTIVE_LEDS - 1) {
            for (int i = 0; i < ACTIVE_LEDS; i++) {
                pixels.setPixelColor(i, toggleState ? adjustBrightness(pixels.Color(255, 255, 0), brightness)
                                                    : adjustBrightness(pixels.Color(0, 255, 0), brightness));
            }
        }
    } else if (floatLevel >= 90) { // Flash even/odd LEDs for over 90
        for (int i = 0; i < NUMPIXELS; i++) {
            if (i % 2 == 0) {
                pixels.setPixelColor(i, toggleState ? adjustBrightness(pixels.Color(255, 255, 255), brightness)
                                                    : adjustBrightness(pixels.Color(0, 0, 0), brightness));
            } else {
                pixels.setPixelColor(i, toggleState ? adjustBrightness(pixels.Color(255, 0, 0), brightness)
                                                    : adjustBrightness(pixels.Color(0, 0, 0), brightness));
            }
        }
    }

    pixels.show(); // Update LED ring
}

void setup() {
    Serial.begin(115200);         // Initialize serial communication
    setupWiFiAndWebServer();      // Set up WiFi and Web Server
    setupOTA();                   // Set up OTA
    setupSocketIO();              // Initialize Socket.IO client

    pixels.begin();               // Initialize NeoPixel
    pixels.show();                // Ensure all LEDs are off initially

    // Initialize float server handlers
    

    // Debugging information
    Serial.println("System initialized.");
    Serial.println("Access OTA at:");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println(":8080/");


}

void loop() {
    otaServer.handleClient();    // Handle OTA requests
    updateLEDs();    
    handleSocketIO();            // Handle Socket.IO communication
 

    

    delay(10);                   // Small delay for stability
}

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
    static int lastFloatLevel = -1;                // Store the previous float level
    static unsigned long lastFlashToggle = 0;      // Track last toggle for flashing
    static unsigned long lastUpdate = 0;           // Last update time for throttling
    static int flashCount = 0;                     // Counter for flashing LEDs when full
    static bool flashDone = false;                 // Track if flashing is complete
    static bool toggleState = false;               // Toggle state for flashing behavior

     if (floatLevel == lastFloatLevel && millis() - lastUpdate < 100) {
        return; // Skip update if nothing has changed or it’s too soon
    }

    lastUpdate = millis();
    lastFloatLevel = floatLevel;

    // Toggle state for flashing behavior
    if (millis() - lastFlashToggle >= flashRate) {
        toggleState = !toggleState;
        lastFlashToggle = millis();
    }

    // Reset all LEDs
    for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, adjustBrightness(pixels.Color(0, 0, 0), brightness));
    }

    // Very low level: All LEDs flash red
    if (floatLevel < 5) {
        for (int i = 0; i < NUMPIXELS; i++) {
            pixels.setPixelColor(i, toggleState ? adjustBrightness(pixels.Color(255, 0, 0), brightness)
                                                : adjustBrightness(pixels.Color(0, 0, 0), brightness));
        }
    }
    // First two LEDs pulse white
    else if (floatLevel <= 10) {
        pixels.setPixelColor(0, toggleState ? adjustBrightness(pixels.Color(255, 255, 255), brightness)
                                            : adjustBrightness(pixels.Color(50, 50, 50), brightness));
        pixels.setPixelColor(1, toggleState ? adjustBrightness(pixels.Color(255, 255, 255), brightness)
                                            : adjustBrightness(pixels.Color(50, 50, 50), brightness));
    }
    // First two LEDs turn green, then illuminate the rest as the level increases
    else if (floatLevel <= 85) {
        // First two LEDs solid green
        pixels.setPixelColor(0, adjustBrightness(pixels.Color(0, 255, 0), brightness));
        pixels.setPixelColor(1, adjustBrightness(pixels.Color(0, 255, 0), brightness));

        // Illuminate additional LEDs based on floatLevel
        int numLit = map(floatLevel, 10, 85, 2, ACTIVE_LEDS);
        for (int i = 2; i < numLit; i++) {
            pixels.setPixelColor(i, adjustBrightness(pixels.Color(0, 255, 0), brightness));
        }
    }
    // Float level at 85 or higher: Flash all 7 LEDs green 3 times, then stay green
    else if (floatLevel > 85 && floatLevel < 90) {
        if (flashCount < 6) { // Flash 3 times (on/off cycle counts as 2)
            for (int i = 0; i < ACTIVE_LEDS; i++) {
                pixels.setPixelColor(i, toggleState ? adjustBrightness(pixels.Color(0, 255, 0), brightness)
                                                    : adjustBrightness(pixels.Color(0, 0, 0), brightness));
            }
            if (!toggleState) flashCount++; // Increment flash count only on "off"
        } else {
            // After flashing 3 times, keep all LEDs solid green
            for (int i = 0; i < ACTIVE_LEDS; i++) {
                pixels.setPixelColor(i, adjustBrightness(pixels.Color(0, 255, 0), brightness));
            }
        }
    }
    // Float level at or above 90: Flash even/odd LEDs
    else if (floatLevel >= 90) {
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

    pixels.show(); // Apply changes to LEDs
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

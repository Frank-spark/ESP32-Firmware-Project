#include <Adafruit_NeoPixel.h>
#include "WiFiSetup.h"
#include "OTASetup.h"
#include "SocketIOSetup.h"
#include <WebServer.h>
#include "GlobalVars.h"

#define PIN        21
#define NUMPIXELS  12
#define ACTIVE_LEDS 7  // Number of active LEDs (1 through 7)
#define SENSOR_PIN 34 // Analog input pin connected to the sensor
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int brightness = 50;
int floatLevel = 0;
const float VCC = 3.3;       // Voltage source (ESP32's 3.3V)
const int ADC_MAX = 4095;    // ESP32 ADC resolution
const float R_FIXED = 120.0; // Fixed resistor value in ohms
unsigned long lastUpdate = 0; // For non-blocking LED updates
bool toggleState = false;     // For flashing LEDs
int flashRate = 750;          // Flashing rate in milliseconds
int flashRate2 = 300;
const float SENSOR_R_MIN = 33.0;  // Minimum resistance (33 ohms)
const float SENSOR_R_MAX = 240.0; // Maximum resistance (240 ohms)
const int numSamples = 10;
int adcReadings[numSamples];
int sampleIndex = 0;

uint32_t adjustBrightness(uint32_t color, int brightness) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    r = (r * brightness) / 255;
    g = (g * brightness) / 255;
    b = (b * brightness) / 255;

    return pixels.Color(r, g, b);
}

// Function to calculate resistance from ADC value
float calculateResistance(int adcValue) {
    float voltage = (adcValue / (float)ADC_MAX) * VCC;
    return (R_FIXED * (VCC - voltage)) / voltage;
}
int getSmoothedADC() {
    adcReadings[sampleIndex] = analogRead(SENSOR_PIN);
    sampleIndex = (sampleIndex + 1) % numSamples;

    long sum = 0;
    for (int i = 0; i < numSamples; i++) sum += adcReadings[i];
    return sum / numSamples;
}
// Function to map resistance to float level (0-100%)
float mapResistanceToFloatLevel(float resistance) {
    if (resistance < SENSOR_R_MIN) resistance = SENSOR_R_MIN;
    if (resistance > SENSOR_R_MAX) resistance = SENSOR_R_MAX;
    return map(resistance, SENSOR_R_MAX, SENSOR_R_MIN, 0, 100);
}

void updateLEDs() {
    static int lastFloatLevel = -1;                // Store the previous float level
    static unsigned long lastFlashToggle = 0;      // Track last toggle for flashing
    static unsigned long lastUpdate = 0;           // Last update time for throttling
    static int flashCount = 0;                     // Counter for flashing LEDs when full
    static bool flashDone = false;                 // Track if flashing is complete
    static bool toggleState = false;               // Toggle state for flashing behavior

    int adcValue = getSmoothedADC();
    float resistance = calculateResistance(adcValue);
    floatLevel = mapResistanceToFloatLevel(resistance);

    if (floatLevel != lastFloatLevel)  {
    emitFloatLevel((int)floatLevel); // Emit float level change
    Serial.printf("[LED Update] Float Level Changed: %.2f\n", floatLevel);
    lastFloatLevel = floatLevel; // Update lastFloatLevel here
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

    // LED behavior based on floatLevel (unchanged logic)
    if (floatLevel < 5) {
        for (int i = 0; i < NUMPIXELS; i++) {
            pixels.setPixelColor(i, toggleState ? adjustBrightness(pixels.Color(255, 0, 0), brightness)
                                                : adjustBrightness(pixels.Color(0, 0, 0), brightness));
        }
    } else if (floatLevel <= 5) {
        pixels.setPixelColor(0, toggleState ? adjustBrightness(pixels.Color(255, 255, 255), brightness)
                                            : adjustBrightness(pixels.Color(50, 50, 50), brightness));
    } else if (floatLevel <= 10) {
        pixels.setPixelColor(0, toggleState ? adjustBrightness(pixels.Color(255, 255, 255), brightness)
                                            : adjustBrightness(pixels.Color(50, 50, 50), brightness));
        pixels.setPixelColor(1, toggleState ? adjustBrightness(pixels.Color(255, 255, 255), brightness)
                                            : adjustBrightness(pixels.Color(50, 50, 50), brightness));
    } else if (floatLevel <= 85) {
        pixels.setPixelColor(0, adjustBrightness(pixels.Color(255, 255, 255), brightness));
        pixels.setPixelColor(1, adjustBrightness(pixels.Color(255, 255, 255), brightness));
        int numLit = map(floatLevel, 10, 85, 2, ACTIVE_LEDS);
        for (int i = 2; i < numLit; i++) {
            pixels.setPixelColor(i, adjustBrightness(pixels.Color(255, 255, 255), brightness));
        }
    } else if (floatLevel > 85 && floatLevel < 90) {
        if (flashCount < 6) {
            for (int i = 0; i < ACTIVE_LEDS; i++) {
                pixels.setPixelColor(i, toggleState ? adjustBrightness(pixels.Color(255, 255, 255), brightness)
                                                    : adjustBrightness(pixels.Color(0, 0, 0), brightness));
            }
            if (!toggleState) flashCount++;
        } else {
            for (int i = 0; i < ACTIVE_LEDS; i++) {
                pixels.setPixelColor(i, adjustBrightness(pixels.Color(255, 255, 255), brightness));
            }
        }
    } else if (floatLevel >= 90) {
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
    analogReadResolution(12); // 12-bit resolution
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

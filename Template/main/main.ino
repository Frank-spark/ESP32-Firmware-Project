#include "WiFiSetup.h"
#include "OTASetup.h"
#include <Adafruit_NeoPixel.h>


#define PIN        21
#define NUMPIXELS  12

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    Serial.begin(115200);
    setupWiFiAndWebServer(); // Call without parameters
    setupOTA();              // Setup OTA
    pixels.begin();          // Initialize NeoPixel
}

void loop() {
    ArduinoOTA.handle();     // Handle OTA updates
    server.handleClient();
    cycleRainbow(); // Run LED rainbow cycle
}

void cycleRainbow() {
  int delayTime = 50; // Time delay between color updates
  // Colors of the rainbow: Red, Orange, Yellow, Green, Blue, Indigo, Violet
  uint32_t colors[] = {
    pixels.Color(255, 0, 0), pixels.Color(255, 165, 0), pixels.Color(255, 255, 0),
    pixels.Color(0, 255, 0), pixels.Color(0, 0, 255), pixels.Color(75, 0, 130),
    pixels.Color(238, 130, 238)
  };
  int colorCount = sizeof(colors) / sizeof(colors[0]);

  // Fade through each color
  for(int i = 0; i < colorCount; i++) {
    for (int brightness = 0; brightness <= 255; brightness += 5) {
      for(int j = 0; j < NUMPIXELS; j++) {
        pixels.setPixelColor(j, pixels.Color((colors[i] >> 16 & 0xFF) * brightness / 255,
                                             (colors[i] >> 8 & 0xFF) * brightness / 255,
                                             (colors[i] & 0xFF) * brightness / 255));
      }
      pixels.show();
      delay(delayTime);
    }
    for (int brightness = 255; brightness >= 0; brightness -= 5) {
      for(int j = 0; j < NUMPIXELS; j++) {
        pixels.setPixelColor(j, pixels.Color((colors[i] >> 16 & 0xFF) * brightness / 255,
                                             (colors[i] >> 8 & 0xFF) * brightness / 255,
                                             (colors[i] & 0xFF) * brightness / 255));
      }
      pixels.show();
      delay(delayTime);
    }
  }
  pixels.clear(); // Clear all LEDs after the cycle
}

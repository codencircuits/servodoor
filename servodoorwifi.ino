#include <FastLED.h>

#define LED_PIN     2        // Pin connected to the data input of the LED matrix
#define NUM_LEDS    512      // Total number of LEDs in a 16x16 matrix (16 * 16)
#define MATRIX_WIDTH  32     // Width of the matrix
#define MATRIX_HEIGHT 16     // Height of the matrix

CRGB leds[NUM_LEDS];

// Mapping function for serpentine layout
int XY(int x, int y) {
  if (y % 2 == 0) {                // Even row
    return y * MATRIX_WIDTH + x;
  } else {                         // Odd row (reversed)
    return y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
  }
}

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(200);      // Increase brightness for a vibrant display
}

void loop() {
  dreamScape();                    // Run the dreamscape animation
}

// Function to create a dreamscape animation
void dreamScape() {
  for (int i = 0; i < NUM_LEDS; i++) {
    // Use a combination of sine waves to create a smooth, organic pulsing effect
    int hue = (sin(i * 0.05) * 128) + 128;
    int brightness = (sin(i * 0.1) + cos(i * 0.07)) * 128 + 128;
    leds[i] = CHSV(hue, 255, brightness);
  }
  FastLED.show();
  delay(20);                     // Adjust delay for speed of effect
}

// Function to clear the matrix
void clearMatrix() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}
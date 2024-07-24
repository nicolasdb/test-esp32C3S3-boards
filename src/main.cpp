#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN     9
#define BRIGHTNESS  25
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define MAX_DIMENSION 25  // Maximum dimension we'll ever use
#define RAINDROP_FREQUENCY 100  // Higher values (up to 255) will create more frequent raindrops

// Rain parameters
#define LIGHT_RAIN 10
#define MEDIUM_RAIN 25
#define HEAVY_RAIN 50

uint8_t currentRainIntensity = MEDIUM_RAIN;
uint8_t dropSpeed = 1;  // Pixels per update
uint8_t updateInterval = 50;  // Milliseconds between updates


enum MatrixOrientation {
  TOP_LEFT_HORIZONTAL,
  TOP_LEFT_VERTICAL,
  BOTTOM_LEFT_HORIZONTAL,
  BOTTOM_LEFT_VERTICAL
};

struct MatrixConfig {
  uint8_t width;
  uint8_t height;
  MatrixOrientation orientation;
  bool zigzag;
};

MatrixConfig matrix = {8, 8, TOP_LEFT_VERTICAL, false};
// with {width, height, orientation mode, zigzag mode} as define just before

#define NUM_LEDS (matrix.width * matrix.height)
CRGB leds[MAX_DIMENSION * MAX_DIMENSION];  // Allocate for maximum possible size
uint8_t raindrops[MAX_DIMENSION] = {0};  // Allocate for maximum possible width



uint16_t XY(uint8_t x, uint8_t y) {
    uint16_t i;
    switch(matrix.orientation) {
        case TOP_LEFT_HORIZONTAL:
            if (matrix.zigzag && y % 2 == 1) {
            x = (matrix.width - 1) - x;
            }
            i = (y * matrix.width) + x;
            break;
        case TOP_LEFT_VERTICAL:
            if (matrix.zigzag && x % 2 == 1) {
            y = (matrix.height - 1) - y;
            }
            i = (x * matrix.height) + y;
            break;
        case BOTTOM_LEFT_HORIZONTAL:
            y = (matrix.height - 1) - y;
            if (matrix.zigzag && y % 2 == 1) {
            x = (matrix.width - 1) - x;
            }
            i = (y * matrix.width) + x;
            break;
        case BOTTOM_LEFT_VERTICAL:
            x = (matrix.width - 1) - x;
            if (matrix.zigzag && x % 2 == 1) {
            y = (matrix.height - 1) - y;
            }
            i = (x * matrix.height) + y;
            break;
    }
    return i;
}

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

// Initialize raindrops array (not strictly necessary with static initialization, but good practice)
  for (int i = 0; i < MAX_DIMENSION; i++) {
    raindrops[i] = 0;
  }
}

// Function to change rain intensity. determines how often new raindrops are created.
void setRainIntensity(uint8_t intensity) {
  currentRainIntensity = intensity;
}

// Function to change drop speed. controls how many pixels a raindrop falls in each update.
void setDropSpeed(uint8_t speed) {
  dropSpeed = speed;
}

void loop() {
  static unsigned long lastUpdate = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastUpdate >= updateInterval) {
    lastUpdate = currentTime;

    FastLED.clear();

    // Update and draw raindrops
    for (int x = 0; x < matrix.width; x++) {
      if (raindrops[x] == 0 && random8() < currentRainIntensity) {
        raindrops[x] = matrix.height;  // Start from just above the top row
      }

      if (raindrops[x] > 0) {
        raindrops[x] = (raindrops[x] > dropSpeed) ? raindrops[x] - dropSpeed : 0;
        if (raindrops[x] < matrix.height) {  // Only draw if within the matrix
          leds[XY(x, raindrops[x])] = CRGB::Blue;
        }
      }
    }

    FastLED.show();
  }
}
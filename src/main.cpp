#include <Arduino.h>
#define FASTLED_INTERNAL  // This suppresses the FastLED pragma messages
#include <FastLED.h>
#include <WiFi.h>
#include "credentials.h"  // Include the credentials header file

// Pin definitions for different boards
#if defined(CONFIG_IDF_TARGET_ESP32C3)
    #define PIN_BUTTON 9
    #define PIN_LED 2
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    #define PIN_BUTTON 7  // External button for esp32-s3-matrix
    #define PIN_LED 14
#elif defined(ARDUINO_M5STACK_STAMPS3)
    #define PIN_BUTTON 0
    #define PIN_LED 21
#else
    #error "Unsupported board selected"
#endif

#define NUM_LEDS 1

CRGB leds[NUM_LEDS];

// Function prototypes
void setupWiFi();
void checkWiFiStatus();
void toggleLED();
void checkButton();
void debugPrint(const String& message);

void setup() {
    Serial.begin(115200);
    debugPrint("ESP32 Board Test - Setup started");

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    FastLED.addLeds<WS2812, PIN_LED, GRB>(leds, NUM_LEDS);
    
    setupWiFi();
    
    debugPrint("Setup completed");
}

void loop() {
    checkWiFiStatus();
    checkButton();
    delay(100);
}

void setupWiFi() {
    debugPrint("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        debugPrint(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        debugPrint("WiFi connected");
        debugPrint("IP address: " + WiFi.localIP().toString());
    } else {
        debugPrint("Failed to connect to WiFi");
    }
}

void checkWiFiStatus() {
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 10000) {  // Check every 10 seconds
        lastCheck = millis();
        if (WiFi.status() == WL_CONNECTED) {
            debugPrint("WiFi still connected");
        } else {
            debugPrint("WiFi disconnected. Attempting to reconnect...");
            setupWiFi();
        }
    }
}

void toggleLED() {
    static bool ledOn = false;
    ledOn = !ledOn;
    leds[0] = ledOn ? CRGB::White : CRGB::Black;
    FastLED.show();
    debugPrint("LED toggled: " + String(ledOn ? "ON" : "OFF"));
}

void checkButton() {
    static bool lastButtonState = HIGH;
    bool buttonState = digitalRead(PIN_BUTTON);
    
    if (buttonState == LOW && lastButtonState == HIGH) {
        delay(50);  // Simple debounce
        if (digitalRead(PIN_BUTTON) == LOW) {
            debugPrint("Button pressed");
            toggleLED();
        }
    }
    lastButtonState = buttonState;
}

void debugPrint(const String& message) {
    Serial.println("[DEBUG] " + message);
}
#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include "WebInterface.h"
#include "RFIDManager.h"
#include "credentials.h"

// Pin definitions for different boards
#if defined(CONFIG_IDF_TARGET_ESP32C3)
    #define PIN_BUTTON 9
    #define PIN_LED 2
    #define PIN_RELAY 3
    #define RST_PIN 10
    #define SS_PIN  7
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    #define PIN_BUTTON 7
    #define PIN_LED 14
    #define PIN_RELAY 15
    #define RST_PIN 10
    #define SS_PIN  11
#elif defined(ARDUINO_M5STACK_STAMPS3)
    #define PIN_BUTTON 0
    #define PIN_LED 21
    #define PIN_RELAY 22
    #define RST_PIN 10
    #define SS_PIN  11
#else
    #error "Unsupported board selected"
#endif

#define NUM_LEDS 1
#define DOOR_OPEN_TIME 5000 // 5 seconds in milliseconds

CRGB leds[NUM_LEDS];
bool doorOpen = false;
unsigned long doorOpenTime = 0;

WebInterface webInterface;
RFIDManager rfidManager(SS_PIN, RST_PIN);

void setupWiFi();
void checkWiFiStatus();
void setDoorState(bool open);
void checkButton();
void checkDoorTimer();
void controlDoorHardware(bool open);

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32 Door Control System - Setup started");

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_RELAY, OUTPUT);
    FastLED.addLeds<WS2812, PIN_LED, GRB>(leds, NUM_LEDS);
    
    if (!SPIFFS.begin(true)) {
        Serial.println("An error has occurred while mounting SPIFFS");
    }
    
    setupWiFi();
    webInterface.setup();
    rfidManager.setup();
    
    Serial.println("Setup completed");
}

void loop() {
    checkWiFiStatus();
    checkButton();
    checkDoorTimer();
    
    if (rfidManager.checkCard()) {
        setDoorState(true);
    }
    
    webInterface.handleClient();
    
    delay(10);
}

void setupWiFi() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected");
        Serial.println("IP address: " + WiFi.localIP().toString());
    } else {
        Serial.println("\nFailed to connect to WiFi");
    }
}

void checkWiFiStatus() {
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 10000) {  // Check every 10 seconds
        lastCheck = millis();
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi disconnected. Attempting to reconnect...");
            setupWiFi();
        }
    }
}

void setDoorState(bool open) {
    doorOpen = open;
    controlDoorHardware(doorOpen);
    if (doorOpen) {
        doorOpenTime = millis();
        Serial.println("Door opened");
    } else {
        Serial.println("Door closed");
    }
    webInterface.sendUpdate(doorOpen);
}

void checkButton() {
    static bool lastButtonState = HIGH;
    bool buttonState = digitalRead(PIN_BUTTON);
    
    if (buttonState == LOW && lastButtonState == HIGH) {
        delay(50);  // Simple debounce
        if (digitalRead(PIN_BUTTON) == LOW) {
            Serial.println("Button pressed");
            setDoorState(true);
        }
    }
    lastButtonState = buttonState;
}

void checkDoorTimer() {
    if (doorOpen && (millis() - doorOpenTime >= DOOR_OPEN_TIME)) {
        setDoorState(false);
    }
}

void controlDoorHardware(bool open) {
    // Control LED
    leds[0] = open ? CRGB::White : CRGB::Black;
    FastLED.show();

    // Control Relay
    digitalWrite(PIN_RELAY, open ? HIGH : LOW);
}

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include "WebInterface.h"
#include "credentials.h"

#define PIN_BUTTON 9
#define PIN_RELAY 3
#define PIN_NEOPIXEL 2
#define NUM_LEDS 1

#define DOOR_OPEN_TIME 5000 // 5 seconds in milliseconds

CRGB leds[NUM_LEDS];

bool doorOpen = false;
unsigned long doorOpenTime = 0;

WebInterface webInterface;

void setDoorState(bool open);
void checkButton();

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32 Door Control System");

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_RELAY, OUTPUT);
    digitalWrite(PIN_RELAY, LOW);

    FastLED.addLeds<NEOPIXEL, PIN_NEOPIXEL>(leds, NUM_LEDS);
    FastLED.setBrightness(50);

    // Initialize SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("An error has occurred while mounting SPIFFS");
        return;
    }

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Initialize WebInterface
    webInterface.setup();

    Serial.printf("PIN_BUTTON: %d, PIN_RELAY: %d, PIN_NEOPIXEL: %d\n", PIN_BUTTON, PIN_RELAY, PIN_NEOPIXEL);
}

void loop() {
    checkButton();
    webInterface.handleClient();

    if (doorOpen && (millis() - doorOpenTime >= DOOR_OPEN_TIME)) {
        setDoorState(false);
    }

    FastLED.show();
    delay(10);
}

void setDoorState(bool open) {
    doorOpen = open;
    doorOpenTime = millis();
    digitalWrite(PIN_RELAY, open ? HIGH : LOW);
    leds[0] = open ? CRGB::Green : CRGB::Red;
    Serial.printf("Door %s\n", open ? "opened" : "closed");
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

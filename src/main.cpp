#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoWebsockets.h>
#include <vector>
#include "credentials.h"

using namespace websockets;

// Pin definitions for different boards
#if defined(CONFIG_IDF_TARGET_ESP32C3)
    #define PIN_BUTTON 9
    #define PIN_LED 2
    #define PIN_RELAY 3  // Assign an appropriate pin for the relay on ESP32-C3
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    #define PIN_BUTTON 7  // External button for esp32-s3-matrix
    #define PIN_LED 14
    #define PIN_RELAY 15  // Assign an appropriate pin for the relay on ESP32-S3
#elif defined(ARDUINO_M5STACK_STAMPS3)
    #define PIN_BUTTON 0
    #define PIN_LED 21
    #define PIN_RELAY 22  // Assign an appropriate pin for the relay on M5Stack Stamp S3
#else
    #error "Unsupported board selected"
#endif

#define NUM_LEDS 1
#define DOOR_OPEN_TIME 5000 // 5 seconds in milliseconds

CRGB leds[NUM_LEDS];
WebServer server(80);
WebsocketsServer webSocketServer;
std::vector<WebsocketsClient> clients;
bool doorOpen = false;
unsigned long doorOpenTime = 0;

// Function prototypes
void setupWiFi();
void checkWiFiStatus();
void setDoorState(bool open);
void checkButton();
void debugPrint(const String& message);
void handleRoot();
void handleDoorOpen();
void setupWebServer();
void checkDoorTimer();
void controlDoorHardware(bool open);
void sendWebSocketUpdate();
void onWebSocketMessage(WebsocketsClient& client, WebsocketsMessage message);

void setup() {
    Serial.begin(115200);
    debugPrint("ESP32 Door Control System - Setup started");

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_RELAY, OUTPUT);
    FastLED.addLeds<WS2812, PIN_LED, GRB>(leds, NUM_LEDS);
    
    setupWiFi();
    setupWebServer();
    
    webSocketServer.listen(81);
    
    debugPrint("Setup completed");
}

void loop() {
    checkWiFiStatus();
    checkButton();
    checkDoorTimer();
    server.handleClient();
    
    if(webSocketServer.poll()) {
        WebsocketsClient client = webSocketServer.accept();
        if(client.available()) {
            client.onMessage(onWebSocketMessage);
            clients.push_back(client);
            debugPrint("New WebSocket client connected");
            client.send(String(doorOpen));
        }
    }
    
    for(auto it = clients.begin(); it != clients.end();) {
        if(it->available()) {
            it->poll();
            ++it;
        } else {
            debugPrint("WebSocket client disconnected");
            it = clients.erase(it);
        }
    }
    
    delay(10);
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

void controlDoorHardware(bool open) {
    // Control LED
    leds[0] = open ? CRGB::White : CRGB::Black;
    FastLED.show();

    // Control Relay
    digitalWrite(PIN_RELAY, open ? HIGH : LOW);
}

void setDoorState(bool open) {
    doorOpen = open;
    controlDoorHardware(doorOpen);
    if (doorOpen) {
        doorOpenTime = millis();
        debugPrint("Door opened");
    } else {
        debugPrint("Door closed");
    }
    sendWebSocketUpdate();
}

void checkButton() {
    static bool lastButtonState = HIGH;
    bool buttonState = digitalRead(PIN_BUTTON);
    
    if (buttonState == LOW && lastButtonState == HIGH) {
        delay(50);  // Simple debounce
        if (digitalRead(PIN_BUTTON) == LOW) {
            debugPrint("Button pressed");
            setDoorState(true);
        }
    }
    lastButtonState = buttonState;
}

void debugPrint(const String& message) {
    Serial.println("[DEBUG] " + message);
}

void handleRoot() {
    String html = R"(
    <html>
    <head>
        <script>
            var ws;
            function connectWebSocket() {
                ws = new WebSocket('ws://' + location.hostname + ':81/');
                ws.onopen = function() {
                    console.log('WebSocket connected');
                    ws.send('getDoorState');
                };
                ws.onmessage = function(event) {
                    console.log('Received:', event.data);
                    var doorOpen = event.data === 'true';
                    document.getElementById('doorStatus').innerText = doorOpen ? 'OPEN' : 'CLOSED';
                };
                ws.onclose = function() {
                    console.log('WebSocket disconnected');
                    setTimeout(connectWebSocket, 2000);
                };
            }
            function openDoor() {
                fetch('/door/open').then(response => {
                    if (!response.ok) {
                        console.error('Failed to open door');
                    }
                });
            }
            window.onload = connectWebSocket;
        </script>
    </head>
    <body>
        <h1>ESP32-C3 Door Control</h1>
        <p>Door is currently <span id='doorStatus'>UNKNOWN</span></p>
        <p><button onclick='openDoor()'>Open Door</button></p>
    </body>
    </html>
    )";
    server.send(200, "text/html", html);
}

void handleDoorOpen() {
    setDoorState(true);
    server.send(200, "text/plain", "Door opened");
}

void setupWebServer() {
    server.on("/", handleRoot);
    server.on("/door/open", HTTP_GET, handleDoorOpen);
    server.begin();
    debugPrint("Web server started");
}

void checkDoorTimer() {
    if (doorOpen && (millis() - doorOpenTime >= DOOR_OPEN_TIME)) {
        setDoorState(false);
    }
}

void sendWebSocketUpdate() {
    String update = String(doorOpen);
    debugPrint("Sending WebSocket update: " + update);
    for(auto& client : clients) {
        if(client.available()) {
            bool sent = client.send(update);
            debugPrint("WebSocket update sent: " + String(sent ? "success" : "failed"));
        }
    }
}

void onWebSocketMessage(WebsocketsClient& client, WebsocketsMessage message) {
    debugPrint("Received WebSocket message: " + message.data());
    if(message.data() == "getDoorState") {
        String state = String(doorOpen);
        bool sent = client.send(state);
        debugPrint("Sent door state '" + state + "' to client: " + String(sent ? "success" : "failed"));
    }
}

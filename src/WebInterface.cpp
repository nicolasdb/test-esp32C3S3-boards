#include "WebInterface.h"
#include <SPIFFS.h>

extern void setDoorState(bool open);

WebInterface::WebInterface() : server(80) {}

void WebInterface::setup() {
    server.on("/", std::bind(&WebInterface::handleRoot, this));
    server.on("/door/open", HTTP_GET, std::bind(&WebInterface::handleDoorOpen, this));
    server.begin();
    webSocketServer.listen(81);
    Serial.println("Web server started");
}

void WebInterface::handleClient() {
    server.handleClient();
    
    if(webSocketServer.poll()) {
        auto client = webSocketServer.accept();
        if(client.available()) {
            client.onMessage(std::bind(&WebInterface::onWebSocketMessage, this, std::placeholders::_1, std::placeholders::_2));
            clients.push_back(client);
            Serial.println("New WebSocket client connected");
        }
    }
    
    for(auto it = clients.begin(); it != clients.end();) {
        if(it->available()) {
            it->poll();
            ++it;
        } else {
            Serial.println("WebSocket client disconnected");
            it = clients.erase(it);
        }
    }
}

void WebInterface::handleRoot() {
    File file = SPIFFS.open("/index.html", "r");
    if (file) {
        server.streamFile(file, "text/html");
        file.close();
    } else {
        server.send(404, "text/plain", "File not found");
    }
}

void WebInterface::handleDoorOpen() {
    setDoorState(true);
    server.send(200, "text/plain", "Door opened");
}

void WebInterface::sendUpdate(bool doorState) {
    String update = String(doorState);
    for(auto& client : clients) {
        if(client.available()) {
            client.send(update);
        }
    }
}

void WebInterface::onWebSocketMessage(websockets::WebsocketsClient& client, websockets::WebsocketsMessage message) {
    if(message.data() == "getDoorState") {
        // You'll need to implement a way to get the current door state here
        // For now, we'll just send a placeholder value
        client.send("false");
    }
}

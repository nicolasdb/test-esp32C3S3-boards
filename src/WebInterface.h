#ifndef WEBINTERFACE_H
#define WEBINTERFACE_H

#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoWebsockets.h>
#include <vector>

class WebInterface {
public:
    WebInterface();
    void setup();
    void handleClient();
    void sendUpdate(bool doorState);

private:
    WebServer server;
    websockets::WebsocketsServer webSocketServer;
    std::vector<websockets::WebsocketsClient> clients;

    void handleRoot();
    void handleDoorOpen();
    void onWebSocketMessage(websockets::WebsocketsClient& client, websockets::WebsocketsMessage message);
};

#endif // WEBINTERFACE_H

#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class WebServerManager
{
public:
    explicit WebServerManager(uint16_t port);
    void begin();
    void broadcastStatus(bool isOccupied, uint32_t visitCount, const String &lastEntry);

private:
    AsyncWebServer _server;
    AsyncWebSocket _ws;

    void setupRoutes();
    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
};
#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <functional>

using CommandCallback = std::function<void(const String& command, const JsonVariant& value)>;

class WebServerManager {
public:
    explicit WebServerManager(uint16_t port);
    
    void begin(CommandCallback onCommandReceived);
    void broadcastTelemetry(bool occupied, uint32_t visitCount, const String& timestamp, 
                           uint32_t activeVisitSec, uint32_t timeoutSec, const String& ledMode);

private:
    AsyncWebServer server;
    AsyncWebSocket ws;
    CommandCallback commandHandler;

    void onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, 
                        AwsEventType type, void* arg, uint8_t* data, size_t len);
};
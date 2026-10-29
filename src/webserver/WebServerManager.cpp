#include "WebServerManager.h"
#include <LittleFS.h>
#include <WiFi.h>
#include "logger/Logger.h"

static const char* TAG = "WebServer";

WebServerManager::WebServerManager(uint16_t port) 
    : server(port), ws("/ws") {}

void WebServerManager::begin(CommandCallback onCommandReceived) {
    this->commandHandler = onCommandReceived;

    if (!LittleFS.begin(true)) {
        Logger::error(TAG, "Failed to mount LittleFS filesystem!");
        return;
    }
    Logger::info(TAG, "LittleFS mounted successfully.");

    ws.onEvent([this](AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
        this->onWebSocketEvent(server, client, type, arg, data, len);
    });
    
    server.addHandler(&ws);
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

    server.begin();
    Logger::info(TAG, "HTTP Server started on port 80.");
}

void WebServerManager::onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, 
                                      AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Logger::info(TAG, "WebSocket client connected: %u", client->id());
    } else if (type == WS_EVT_DATA) {
        AwsFrameInfo* info = (AwsFrameInfo*)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, data, len);
            if (!err && commandHandler) {
                const char* cmd = doc["command"];
                commandHandler(String(cmd), doc["value"]);
            }
        }
    }
}

void WebServerManager::broadcastTelemetry(bool occupied, uint32_t visitCount, const String& timestamp, 
                                       uint32_t activeVisitSec, uint32_t timeoutSec, const String& ledMode) {
    if (ws.count() == 0) return;

    JsonDocument doc;
    doc["occupied"] = occupied;
    doc["visitCount"] = visitCount;
    doc["timestamp"] = timestamp;
    doc["activeVisitSec"] = activeVisitSec;
    doc["rssi"] = WiFi.RSSI();
    doc["freeHeap"] = ESP.getFreeHeap();
    doc["uptimeSec"] = millis() / 1000;
    doc["ledTimeoutSec"] = timeoutSec;
    doc["ledMode"] = ledMode;

    String jsonBuffer;
    serializeJson(doc, jsonBuffer);
    ws.textAll(jsonBuffer);
}
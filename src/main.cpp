#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "logger/Logger.h"
#include "time/TimeManager.h"
#include "motion/MotionManager.h"
#include "firebase/FirebaseManager.h"
#include "webserver/WebServerManager.h"

static const char* TAG = "Main";

MotionManager motionModule(Config::Hardware::PIN_PIR, Config::Hardware::PIN_LED, Config::Timing::LED_TIMEOUT_MS);
FirebaseManager firebaseModule;
WebServerManager webServerModule(Config::WebServer::PORT);

uint32_t telemetryIntervalMs = 2000;
uint32_t lastTelemetryTime = 0;

void handleWebCommand(const String& command, const JsonVariant& value) {
    if (command == "setLedMode") {
        String modeStr = value.as<String>();
        motionModule.setLedMode(modeStr);
        Logger::info(TAG, "Web Command: LED Mode set to %s", modeStr.c_str());
    } else if (command == "setTimeout") {
        uint32_t newTimeoutSec = value.as<uint32_t>();
        motionModule.setTimeoutMs(newTimeoutSec * 1000);
        Logger::info(TAG, "Web Command: Motion timeout set to %us", newTimeoutSec);
    }
}

void setup() {
    Logger::begin();
    Logger::info(TAG, "Starting Solaris IoT Firmware...");

    // 1. Initialize Motion Hardware
    motionModule.begin();

    // 2. Initialize Wi-Fi driver FIRST (creates FreeRTOS network queues)
    WiFi.mode(WIFI_STA);

    // 3. Mount LittleFS and start Async Web Server
    webServerModule.begin(handleWebCommand);

    // 4. Connect to local Wi-Fi network
    WiFi.begin(Config::Wifi::SSID, Config::Wifi::PASSWORD);
    Logger::info(TAG, "Connecting to Wi-Fi...");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    Logger::info(TAG, "Wi-Fi connected! IP: %s", WiFi.localIP().toString().c_str());

    // 5. Initialize Services relying on network
    TimeManager::begin();
    firebaseModule.begin();

    Logger::info(TAG, "System initialization complete.");
}

void loop() {
    MotionEvent event = motionModule.update();

    if (event == MotionEvent::VISIT_STARTED) {
        Logger::info(TAG, "Motion detected. Room marked OCCUPIED.");
    } 
    else if (event == MotionEvent::VISIT_ENDED) {
        Logger::info(TAG, "Motion timeout reached. Room marked VACANT.");
        VisitData visit = motionModule.getCompletedVisitData();
        firebaseModule.logVisit(visit);
    }

    // Periodic telemetry broadcast every 2 seconds
    if (millis() - lastTelemetryTime >= telemetryIntervalMs) {
        lastTelemetryTime = millis();
        webServerModule.broadcastTelemetry(
            motionModule.isOccupied(),
            motionModule.getDailyVisitCount(),
            TimeManager::getTimestamp(),
            motionModule.getActiveVisitDurationSec(),
            motionModule.getTimeoutMs() / 1000,
            motionModule.getLedModeStr()
        );
    }
}
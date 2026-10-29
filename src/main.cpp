#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "logger/Logger.h"
#include "time/TimeManager.h"
#include "motion/MotionManager.h"
#include "firebase/FirebaseManager.h"
#include "webserver/WebServerManager.h"

static const char* TAG = "Main";

WiFiClient wifiClient;
MotionManager motionModule(Config::Hardware::PIN_PIR, Config::Hardware::PIN_LED, Config::Timing::LED_TIMEOUT_MS);
FirebaseManager firebaseModule;
WebServerManager webServerModule(Config::WebServer::PORT);

void setupNetwork() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(Config::Wifi::SSID, Config::Wifi::PASSWORD);
    Logger::info(TAG, "Connecting to Wi-Fi...");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    
    Logger::info(TAG, "Wi-Fi connected! IP: " + WiFi.localIP().toString());
}

void setup() {
    Logger::begin();
    Logger::info(TAG, "Initializing Solaris IoT Firmware...");

    motionModule.begin();
    setupNetwork();

    TimeManager::begin();
    firebaseModule.begin();
    webServerModule.begin();

    Logger::info(TAG, "System initialization complete.");
}

void loop() {
    MotionEvent event = motionModule.update();

    if (event == MotionEvent::VISIT_STARTED) {
        Logger::info(TAG, "Motion detected. Room marked OCCUPIED.");
        
        webServerModule.broadcastStatus(
            true, 
            motionModule.getDailyVisitCount(), 
            TimeManager::getTimestamp()
        );
    } 
    else if (event == MotionEvent::VISIT_ENDED) {
        Logger::info(TAG, "Motion timeout reached. Room marked VACANT.");
        
        VisitData visit = motionModule.getCompletedVisitData();
        uint32_t visitCount = motionModule.getDailyVisitCount();

        Logger::debug(TAG, "Visit duration: " + String(visit.durationSeconds) + " seconds.");

        firebaseModule.logVisit(visit);
        webServerModule.broadcastStatus(
            false, 
            visitCount, 
            visit.timestampStart
        );
    }
}
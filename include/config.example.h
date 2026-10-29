#pragma once
#include <Arduino.h>

enum class LogLevel {
    NONE = 0,
    ERROR = 1,
    WARN = 2,
    INFO = 3,
    DEBUG = 4
};

namespace Config {
    namespace Logging {
        constexpr LogLevel ACTIVE_LEVEL = LogLevel::INFO;
        constexpr uint32_t BAUD_RATE = 115200;
    }

    namespace Wifi {
        constexpr const char* SSID = "YOUR_WIFI_SSID";
        constexpr const char* PASSWORD = "YOUR_WIFI_PASSWORD";
    }

    namespace Firebase {
        constexpr const char* DATABASE_URL = "YOUR_PROJECT_ID-default-rtdb.firebaseio.com";
        constexpr const char* DATABASE_SECRET = "YOUR_FIREBASE_DATABASE_SECRET";
        constexpr uint32_t SERVER_TIMEOUT_MS = 15000;
        constexpr uint32_t SOCKET_TIMEOUT_MS = 10000;
        constexpr uint16_t RX_BUFFER_SIZE = 2048;
        constexpr uint16_t TX_BUFFER_SIZE = 1024;
    }

    namespace WebServer {
        constexpr uint16_t PORT = 80;
    }

    namespace Hardware {
        constexpr uint8_t PIN_PIR = 13;
        constexpr uint8_t PIN_LED = 12;
    }

    namespace Timing {
        constexpr uint32_t LED_TIMEOUT_MS = 10000;
    }

    namespace Ntp {
        constexpr const char* SERVER = "pool.ntp.org";
        constexpr int32_t GMT_OFFSET_SEC = -10800; // UTC-3 (Brasília)
        constexpr int32_t DAYLIGHT_OFFSET_SEC = 0;
    }
}
#include "TimeManager.h"
#include "config.h"

void TimeManager::begin()
{
    configTime(Config::Ntp::GMT_OFFSET_SEC, Config::Ntp::DAYLIGHT_OFFSET_SEC, Config::Ntp::SERVER);
}

String TimeManager::getTimestamp()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("[TimeManager] Error: Failed to obtain NTP time.");
        return "";
    }
    char buffer[30];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return String(buffer);
}

float TimeManager::convertTimeToDecimal(const struct tm *timeinfo)
{
    if (!timeinfo)
        return 0.0f;
    return timeinfo->tm_hour + (timeinfo->tm_min / 60.0f) + (timeinfo->tm_sec / 3600.0f);
}

struct tm TimeManager::parseTimestamp(const String &timestampStr)
{
    struct tm timeStruct;
    memset(&timeStruct, 0, sizeof(struct tm));
    strptime(timestampStr.c_str(), "%Y-%m-%d %H:%M:%S", &timeStruct);
    return timeStruct;
}
#pragma once
#include <Arduino.h>
#include <time.h>

class TimeManager
{
public:
    static void begin();
    static String getTimestamp();
    static float convertTimeToDecimal(const struct tm *timeinfo);
    static struct tm parseTimestamp(const String &timestampStr);
};
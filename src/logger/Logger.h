#pragma once
#include <Arduino.h>
#include "config.h"

class Logger
{
public:
    static void begin(uint32_t baudRate = Config::Logging::BAUD_RATE);

    static void error(const char *tag, const String &message);
    static void warn(const char *tag, const String &message);
    static void info(const char *tag, const String &message);
    static void debug(const char *tag, const String &message);

private:
    static void log(LogLevel level, const char *tag, const String &message);
};
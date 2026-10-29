#include "Logger.h"

void Logger::begin(uint32_t baudRate)
{
    Serial.begin(baudRate);
}

void Logger::log(LogLevel level, const char *tag, const String &message)
{
    if (level > Config::Logging::ACTIVE_LEVEL || Config::Logging::ACTIVE_LEVEL == LogLevel::NONE)
    {
        return;
    }

    const char *levelStr = "";
    switch (level)
    {
    case LogLevel::ERROR:
        levelStr = "ERROR";
        break;
    case LogLevel::WARN:
        levelStr = "WARN ";
        break;
    case LogLevel::INFO:
        levelStr = "INFO ";
        break;
    case LogLevel::DEBUG:
        levelStr = "DEBUG";
        break;
    default:
        break;
    }

    Serial.printf("[%s] [%s] %s\n", levelStr, tag, message.c_str());
}

void Logger::error(const char *tag, const String &message) { log(LogLevel::ERROR, tag, message); }
void Logger::warn(const char *tag, const String &message) { log(LogLevel::WARN, tag, message); }
void Logger::info(const char *tag, const String &message) { log(LogLevel::INFO, tag, message); }
void Logger::debug(const char *tag, const String &message) { log(LogLevel::DEBUG, tag, message); }
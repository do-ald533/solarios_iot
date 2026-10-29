#include "Logger.h"
#include <stdarg.h>

void Logger::begin(uint32_t baudRate) {
    Serial.begin(baudRate);
}

void Logger::log(LogLevel level, const char* tag, const char* format, va_list args) {
    if (level > Config::Logging::ACTIVE_LEVEL || Config::Logging::ACTIVE_LEVEL == LogLevel::NONE) {
        return;
    }

    const char* levelStr = "";
    switch (level) {
        case LogLevel::ERROR: levelStr = "ERROR"; break;
        case LogLevel::WARN:  levelStr = "WARN "; break;
        case LogLevel::INFO:  levelStr = "INFO "; break;
        case LogLevel::DEBUG: levelStr = "DEBUG"; break;
        default: break;
    }

    Serial.printf("[%s] [%s] ", levelStr, tag);
    
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    Serial.println(buffer);
}

void Logger::error(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LogLevel::ERROR, tag, format, args);
    va_end(args);
}

void Logger::warn(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LogLevel::WARN, tag, format, args);
    va_end(args);
}

void Logger::info(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LogLevel::INFO, tag, format, args);
    va_end(args);
}

void Logger::debug(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LogLevel::DEBUG, tag, format, args);
    va_end(args);
}
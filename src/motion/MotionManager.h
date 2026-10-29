#pragma once
#include <Arduino.h>
#include <Preferences.h>

enum class MotionEvent {
    NONE,
    VISIT_STARTED,
    VISIT_ENDED
};

enum class LedMode {
    AUTO,
    FORCE_ON,
    FORCE_OFF
};

struct VisitData {
    float durationSeconds;
    String timestampStart;
    String timestampEnd;
};

class MotionManager {
public:
    MotionManager(uint8_t pirPin, uint8_t ledPin, uint32_t timeoutMs);
    
    void begin();
    MotionEvent update();
    
    VisitData getCompletedVisitData() const;
    uint32_t getDailyVisitCount() const;
    bool isOccupied() const;
    uint32_t getActiveVisitDurationSec() const;
    uint32_t getTimeoutMs() const;
    
    void setTimeoutMs(uint32_t newTimeoutMs);
    void setLedMode(LedMode mode);
    void setLedMode(const String& modeStr);
    
    LedMode getLedMode() const;
    String getLedModeStr() const;
    void resetDailyVisitCount();

private:
    uint8_t _pirPin;
    uint8_t _ledPin;
    uint32_t _timeoutMs;
    bool _roomOccupied;
    LedMode _ledMode;
    uint32_t _lastMotionTime;
    uint32_t _startTimeMillis;
    uint32_t _dailyVisitCounter;
    String _entryTimestamp;
    
    VisitData _lastCompletedVisit;
    Preferences _prefs;
};
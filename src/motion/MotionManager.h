#pragma once
#include <Arduino.h>
#include <Preferences.h>

enum class MotionEvent
{
    NONE,
    VISIT_STARTED,
    VISIT_ENDED
};

struct VisitData
{
    String timestampStart;
    String timestampEnd;
    float durationSeconds;
};

class MotionManager
{
public:
    explicit MotionManager(uint8_t pirPin, uint8_t ledPin, uint32_t timeoutMs);
    void begin();
    MotionEvent update();

    VisitData getCompletedVisitData() const;
    uint32_t getDailyVisitCount() const;
    bool isOccupied() const;
    void resetDailyVisitCount();

private:
    uint8_t _pirPin;
    uint8_t _ledPin;
    uint32_t _timeoutMs;

    bool _ledActive;
    uint32_t _lastMotionTime;
    uint32_t _startTimeMillis;

    String _entryTimestamp;
    uint32_t _dailyVisitCounter;
    VisitData _lastCompletedVisit;
    Preferences _prefs;
};
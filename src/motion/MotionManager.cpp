#include "MotionManager.h"
#include "time/TimeManager.h"

MotionManager::MotionManager(uint8_t pirPin, uint8_t ledPin, uint32_t timeoutMs)
    : _pirPin(pirPin),
      _ledPin(ledPin),
      _timeoutMs(timeoutMs),
      _ledActive(false),
      _lastMotionTime(0),
      _startTimeMillis(0),
      _dailyVisitCounter(0) {}

void MotionManager::begin()
{
    pinMode(_pirPin, INPUT);
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW);

    // Load persistent visit count from Flash memory
    _prefs.begin("solaris", false);
    _dailyVisitCounter = _prefs.getUInt("visits", 0);
    _prefs.end();
}

MotionEvent MotionManager::update()
{
    int pirReading = digitalRead(_pirPin);
    uint32_t now = millis();

    if (pirReading == HIGH)
    {
        _lastMotionTime = now;
        if (!_ledActive)
        {
            _ledActive = true;
            digitalWrite(_ledPin, HIGH);
            _startTimeMillis = now;
            _entryTimestamp = TimeManager::getTimestamp();

            _dailyVisitCounter++;

            // Persist count to NVS memory across reboots
            _prefs.begin("solaris", false);
            _prefs.putUInt("visits", _dailyVisitCounter);
            _prefs.end();

            return MotionEvent::VISIT_STARTED;
        }
    }

    if (_ledActive && (now - _lastMotionTime >= _timeoutMs))
    {
        _ledActive = false;
        digitalWrite(_ledPin, LOW);

        uint32_t durationMillis = now - _startTimeMillis;
        String exitTimestamp = TimeManager::getTimestamp();

        _lastCompletedVisit.timestampStart = _entryTimestamp;
        _lastCompletedVisit.timestampEnd = exitTimestamp;
        _lastCompletedVisit.durationSeconds = durationMillis / 1000.0f;

        return MotionEvent::VISIT_ENDED;
    }

    return MotionEvent::NONE;
}

VisitData MotionManager::getCompletedVisitData() const
{
    return _lastCompletedVisit;
}

uint32_t MotionManager::getDailyVisitCount() const
{
    return _dailyVisitCounter;
}

bool MotionManager::isOccupied() const
{
    return _ledActive;
}

void MotionManager::resetDailyVisitCount()
{
    _dailyVisitCounter = 0;
    _prefs.begin("solaris", false);
    _prefs.putUInt("visits", 0);
    _prefs.end();
}
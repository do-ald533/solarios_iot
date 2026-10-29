#include "MotionManager.h"
#include "time/TimeManager.h"

MotionManager::MotionManager(uint8_t pirPin, uint8_t ledPin, uint32_t timeoutMs)
    : _pirPin(pirPin),
      _ledPin(ledPin),
      _timeoutMs(timeoutMs),
      _roomOccupied(false),
      _ledMode(LedMode::AUTO),
      _lastMotionTime(0),
      _startTimeMillis(0),
      _dailyVisitCounter(0) {}

void MotionManager::begin()
{
    pinMode(_pirPin, INPUT);
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW);

    _prefs.begin("solaris", false);
    _dailyVisitCounter = _prefs.getUInt("visits", 0);
    _timeoutMs = _prefs.getUInt("timeout", _timeoutMs);
    _prefs.end();
}

MotionEvent MotionManager::update()
{
    uint32_t now = millis();
    int pirReading = digitalRead(_pirPin);
    MotionEvent event = MotionEvent::NONE;

    // 1. Evaluate PIR Sensor & Room Occupancy (Independent of LED Mode)
    if (pirReading == HIGH)
    {
        _lastMotionTime = now;
        if (!_roomOccupied)
        {
            _roomOccupied = true;
            _startTimeMillis = now;
            _entryTimestamp = TimeManager::getTimestamp();

            _dailyVisitCounter++;

            _prefs.begin("solaris", false);
            _prefs.putUInt("visits", _dailyVisitCounter);
            _prefs.end();

            event = MotionEvent::VISIT_STARTED;
        }
    }

    if (_roomOccupied && (now - _lastMotionTime >= _timeoutMs))
    {
        _roomOccupied = false;

        uint32_t durationMillis = now - _startTimeMillis;
        String exitTimestamp = TimeManager::getTimestamp();

        _lastCompletedVisit.timestampStart = _entryTimestamp;
        _lastCompletedVisit.timestampEnd = exitTimestamp;
        _lastCompletedVisit.durationSeconds = durationMillis / 1000.0f;

        event = MotionEvent::VISIT_ENDED;
    }

    // 2. Control Hardware LED output based on selected LedMode
    switch (_ledMode)
    {
        case LedMode::FORCE_ON:
            digitalWrite(_ledPin, HIGH);
            break;
        case LedMode::FORCE_OFF:
            digitalWrite(_ledPin, LOW);
            break;
        case LedMode::AUTO:
        default:
            digitalWrite(_ledPin, _roomOccupied ? HIGH : LOW);
            break;
    }

    return event;
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
    return _roomOccupied;
}

void MotionManager::resetDailyVisitCount()
{
    _dailyVisitCounter = 0;
    _prefs.begin("solaris", false);
    _prefs.putUInt("visits", 0);
    _prefs.end();
}

void MotionManager::setTimeoutMs(uint32_t newTimeoutMs)
{
    _timeoutMs = newTimeoutMs;
    _prefs.begin("solaris", false);
    _prefs.putUInt("timeout", _timeoutMs);
    _prefs.end();
}

uint32_t MotionManager::getTimeoutMs() const
{
    return _timeoutMs;
}

void MotionManager::setLedMode(LedMode mode)
{
    _ledMode = mode;
}

void MotionManager::setLedMode(const String& modeStr)
{
    if (modeStr == "FORCE_ON") {
        setLedMode(LedMode::FORCE_ON);
    } else if (modeStr == "FORCE_OFF") {
        setLedMode(LedMode::FORCE_OFF);
    } else {
        setLedMode(LedMode::AUTO);
    }
}

LedMode MotionManager::getLedMode() const
{
    return _ledMode;
}

String MotionManager::getLedModeStr() const
{
    switch (_ledMode) {
        case LedMode::FORCE_ON:  return "FORCE_ON";
        case LedMode::FORCE_OFF: return "FORCE_OFF";
        default:                 return "AUTO";
    }
}

uint32_t MotionManager::getActiveVisitDurationSec() const
{
    if (!_roomOccupied) return 0;
    return (millis() - _startTimeMillis) / 1000;
}
#pragma once
#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include "motion/MotionManager.h"

class FirebaseManager {
public:
    void begin();
    bool logVisit(const VisitData& visit);

private:
    FirebaseData _fbdo;
    FirebaseAuth _auth;
    FirebaseConfig _config;
};
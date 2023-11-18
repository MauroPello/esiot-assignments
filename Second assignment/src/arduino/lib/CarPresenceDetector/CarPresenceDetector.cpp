#include <Arduino.h>
#include <CarPresenceDetector.hpp>

CarPresenceDetector::CarPresenceDetector(int outPin) : outPin{outPin} {
    pinMode(outPin, INPUT);
}

bool CarPresenceDetector::detectPresence() {
    return digitalRead(this->outPin) == HIGH;
}


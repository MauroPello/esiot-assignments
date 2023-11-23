#include <Arduino.h>
#include <StartActuator.hpp>

StartActuator::StartActuator(int pin) : pin{pin} { pinMode(pin, INPUT); }

bool StartActuator::isActive() {
    if (digitalRead(pin) == HIGH) {
        return true;
    } else {
        return false;
    }
}

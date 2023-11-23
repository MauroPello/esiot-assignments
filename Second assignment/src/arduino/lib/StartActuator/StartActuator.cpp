#include <Arduino.h>
#include <StartActuator.hpp>

StartActuator::StartActuator(int pin) : pin{pin} { pinMode(pin, INPUT); }

bool StartActuator::isActive() {
    return digitalRead(pin) == HIGH;
}

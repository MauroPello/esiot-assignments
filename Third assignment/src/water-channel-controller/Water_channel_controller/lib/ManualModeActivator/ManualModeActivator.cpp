#include <Arduino.h>
#include <ManualModeActivator.hpp>

ManualModeActivator::ManualModeActivator(int pin) : pin{pin} { pinMode(pin, INPUT); }

bool ManualModeActivator::isActive() {
    return digitalRead(pin) == HIGH;
}

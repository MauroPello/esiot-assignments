#include <Arduino.h>
#include <ManualModeActivator.hpp>
#include <EnableInterrupt.h>


ManualModeActivator::ManualModeActivator(int pin, void (*handler)(void)) : pin(pin) {
    pinMode(pin, INPUT);
    enableInterrupt(pin, handler, RISING);
}


bool ManualModeActivator::isActive() {
    return digitalRead(pin) == HIGH;
}

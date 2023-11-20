#include <Led.hpp>
#include <Arduino.h>

Led::Led(int pin) : pin{pin} {
    pinMode(pin, OUTPUT);
}

void Led::switchOn() {
    digitalWrite(pin, HIGH);
}

void Led::switchOff() {
    digitalWrite(pin, LOW);
};
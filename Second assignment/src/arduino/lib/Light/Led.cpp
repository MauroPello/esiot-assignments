#include <Arduino.h>
#include <Led.hpp>


Led::Led(int pin) : pin{pin} { pinMode(pin, OUTPUT); }

void Led::switchOn() { digitalWrite(pin, HIGH); }

void Led::switchOff() { digitalWrite(pin, LOW); };
#include <Arduino.h>
#include <TemperatureSensor.hpp>

TemperatureSensor::TemperatureSensor(int pin) : pin{pin} {
    pinMode(pin, INPUT);
}

float TemperatureSensor::read() {
    return analogRead(pin) * 0.48828125;
}
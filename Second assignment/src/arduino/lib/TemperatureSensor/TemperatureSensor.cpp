#include <Arduino.h>
#include <TemperatureSensor.hpp>
#define SENSOR_VOLTAGE 3.3
#define SENSOR_RESOLUTION 1024
#define CONVERSION_FACTOR SENSOR_VOLTAGE / SENSOR_RESOLUTION

TemperatureSensor::TemperatureSensor(int pin) : pin{pin} {
    pinMode(pin, INPUT);
}

float TemperatureSensor::read() {
    return analogRead(pin) * CONVERSION_FACTOR;
}
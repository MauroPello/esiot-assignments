#include <Arduino.h>
#include <TemperatureSensor.hpp>
#define SENSOR_VOLTAGE 3.3
#define SENSOR_RESOLUTION 1024.0
#define CONVERSION_FACTOR SENSOR_VOLTAGE / SENSOR_RESOLUTION

TemperatureSensor::TemperatureSensor(int pin) : pin{pin} {
    pinMode(pin, INPUT);
}

float TemperatureSensor::read() {
    float vout = analogRead(pin) * CONVERSION_FACTOR;
    // Serial.println("V: " + String(analogRead(pin)));
    // Serial.println("T: " + String((vout - 0.5) * 100));
    return (vout - 0.5) * 100;
}
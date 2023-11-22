#include <Arduino.h>
#include <TemperatureSensor.hpp>
#define SENSOR_VOLTAGE 3.3
#define SENSOR_RESOLUTION 1024
#define CONVERSION_FACTOR SENSOR_VOLTAGE / SENSOR_RESOLUTION

TemperatureSensor::TemperatureSensor(int pin) : pin{pin} {
    analogReference(EXTERNAL);
    pinMode(pin, INPUT);
}

float TemperatureSensor::read() {
    int sensorValue = analogRead(pin);
    float voltage = sensorValue * CONVERSION_FACTOR;
    float temperature = (voltage - 0.5) * 100;
    return temperature;
}
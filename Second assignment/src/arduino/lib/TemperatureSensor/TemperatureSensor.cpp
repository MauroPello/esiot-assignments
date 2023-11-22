#include <TemperatureSensor.hpp>
#include <Arduino.h>

#define MAX_MEASUREMENTS 10
#define SENSOR_VOLTAGE 3.3
#define SENSOR_RESOLUTION 1024.0
#define CONVERSION_FACTOR SENSOR_VOLTAGE / SENSOR_RESOLUTION

TemperatureSensor::TemperatureSensor(int pin) : pin{pin}, measurementsSum{0}, measurements{MAX_MEASUREMENTS} {
    pinMode(pin, INPUT);
}

float TemperatureSensor::read() {
    float temperature = ((analogRead(pin) * CONVERSION_FACTOR) - 0.5) * 100;

    if (this->measurements.isFull()) {
        measurementsSum -= this->measurements.dequeue();
    }
    this->measurements.enqueue(temperature);
    measurementsSum += temperature;
    return measurementsSum / this->measurements.itemCount();
}
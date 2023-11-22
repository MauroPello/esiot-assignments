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
    int val_Adc = 0;
    for(byte Ciclo = 0; Ciclo < 100; Ciclo++) {
        val_Adc += analogRead(pin);
        delay(10);
    }
    val_Adc /= 100;
    temp = ((val_Adc * CONVERSION_FACTOR) - 0.5) / 0.01;
    return temp;
}
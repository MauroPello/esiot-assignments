#ifndef TEMPERATURE_SENSOR_HPP
#define TEMPERATURE_SENSOR_HPP

class TemperatureSensor {
    const int pin;
    public:
        TemperatureSensor(int pin);
        float read();
};

#endif
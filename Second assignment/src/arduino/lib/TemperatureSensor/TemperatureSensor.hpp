#ifndef TEMPERATURE_SENSOR_HPP
#define TEMPERATURE_SENSOR_HPP
#include <ArduinoQueue.h>

class TemperatureSensor {
  private:
    const int pin;
    float measurementsSum;
    ArduinoQueue<float> measurements;

  public:
    TemperatureSensor(int pin);
    float read();
};

#endif
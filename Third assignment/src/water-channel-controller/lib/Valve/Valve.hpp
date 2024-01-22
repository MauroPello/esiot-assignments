#ifndef VALVE_HPP
#define VALVE_HPP
#include <Servo.h>

class Valve {
  private:
    const int pin;
    Servo servo;

  public:
    Valve(int pin);
    void setLevel(int percentage);
};

#endif
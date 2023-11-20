#ifndef GATE_HPP
#define GATE_HPP
#include <Servo.h>

class Gate {
    const int pin;
    Servo servo;
public:
    Gate(int pin);
    void open();
    void close();
};

#endif
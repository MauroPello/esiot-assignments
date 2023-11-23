#include <Arduino.h>
#include <Gate.hpp>

#define DEGREES_GATE_CLOSED 0
#define DEGREES_GATE_OPEN 97

Gate::Gate(int pin) : pin{pin}, servo{} {
    this->servo.attach(pin);
    this->close();
}

void Gate::open() { this->servo.write(DEGREES_GATE_OPEN); }

void Gate::close() { this->servo.write(DEGREES_GATE_CLOSED); }
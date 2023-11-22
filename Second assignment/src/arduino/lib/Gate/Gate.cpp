#include <Arduino.h>
#include <Gate.hpp>

Gate::Gate(int pin) : pin{pin}, servo{} {
    this->servo.attach(pin);
}

void Gate::open() {
    this->servo.write(100);
}

void Gate::close() {
    this->servo.write(0);
}
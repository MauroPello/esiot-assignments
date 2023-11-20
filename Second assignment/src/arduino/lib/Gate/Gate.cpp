#include <Arduino.h>
#include <Gate.hpp>

Gate::Gate(int pin) : pin{pin}, servo{} {
    this->servo.attach(pin);
}

void Gate::open() {
    this->servo.write(90);
}

void Gate::close() {
    this->servo.write(0);
}
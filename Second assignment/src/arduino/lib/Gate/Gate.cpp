#include <Arduino.h>
#include "Gate.h"

Gate::Gate(int pin) : pin{pin}, servo{} {
    this->servo.attach(pin);
}

void Gate::open() {
    this->servo.write(90);
}

void Gate::close() {
    this->servo.write(0);
}
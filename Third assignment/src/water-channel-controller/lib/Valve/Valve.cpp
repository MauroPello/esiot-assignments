#include <Arduino.h>
#include <Valve.hpp>

Valve::Valve(int pin) : pin{pin}, servo{} {
    this->servo.attach(pin);
}

void Valve::setLevel(int percentage) {
    if (percentage <= 100 && percentage >= 0){
        this->servo.write(map(percentage, 0, 100, 0, 180));
        delay(100);
    }
}

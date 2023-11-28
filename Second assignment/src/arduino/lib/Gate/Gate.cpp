#include <Arduino.h>
#include <Gate.hpp>

#define SERVO_CHANGE_STEP 1
#define SERVO_WAIT_BETWEEN_CHANGES 15
#define DEGREES_GATE_CLOSED 97
#define DEGREES_GATE_OPEN 0

Gate::Gate(int pin) : pin{pin}, servo{} {
    this->servo.attach(pin);
    this->close();
}

void Gate::open() {
    for (int pos = DEGREES_GATE_CLOSED; pos >= DEGREES_GATE_OPEN;
         pos -= SERVO_CHANGE_STEP) {
        this->servo.write(pos);
        delay(SERVO_WAIT_BETWEEN_CHANGES);
    }
}

void Gate::close() {
    for (int pos = DEGREES_GATE_OPEN; pos <= DEGREES_GATE_CLOSED;
         pos += SERVO_CHANGE_STEP) {
        this->servo.write(pos);
        delay(SERVO_WAIT_BETWEEN_CHANGES);
    }
}
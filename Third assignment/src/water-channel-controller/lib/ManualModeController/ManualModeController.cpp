#include <Arduino.h>
#include <ManualModeController.hpp>

int lastValue = 0;

ManualModeController::ManualModeController(int pin) : pin{pin} { pinMode(pin, INPUT); }

int ManualModeController::getValveLevel() {
    int value = analogRead(pin);
    Serial.println("Value: " + String(value) + " Last Value: " + String(lastValue));
    if(lastValue + 10 <= value || lastValue - 10 >= value) {
        lastValue = value;
        return map(value, 0, 1023, -1, 100) + 1;
    } else {
        return map(lastValue, 0, 1023, -1, 100) + 1;
    }
}

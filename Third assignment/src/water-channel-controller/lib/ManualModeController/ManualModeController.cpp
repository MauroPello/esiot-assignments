#include <Arduino.h>
#include <ManualModeController.hpp>

int lastValue = 0;

ManualModeController::ManualModeController(int pin) : pin{pin} { pinMode(pin, INPUT); }

int ManualModeController::getValveLevel() {
    const int numReadings = 10;
    int total = 0;
    for (int i = 0; i < numReadings; ++i) {
        total += analogRead(pin);
        delay(50);
    }
    double average = total / numReadings;
    return round(average * 100.0 / 1024.0);
}

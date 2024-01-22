#include <Arduino.h>
#include <ManualModeController.hpp>

ManualModeController::ManualModeController(int pin) : pin{pin} { pinMode(pin, INPUT); }

int ManualModeController::getValveLevel() {
    return map (analogRead(pin), 0, 1023, 1, 100);
}

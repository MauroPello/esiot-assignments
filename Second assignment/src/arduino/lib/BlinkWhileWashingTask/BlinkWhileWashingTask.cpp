#include <BlinkWhileWashingTask.hpp>

void blinkWhileWashing() {
    switch (context.blinkWhileWashingState) {
    case LED2_OFF:
        if (context.carWashingSystemState == WASHING) {
            context.blinkWhileWashingState = LED2_ON;
            context.led2->switchOn();
        }
        break;
    case LED2_ON:
        context.blinkWhileWashingState = LED2_OFF;
        context.led2->switchOff();
        break;
    default:
        break;
    }
}

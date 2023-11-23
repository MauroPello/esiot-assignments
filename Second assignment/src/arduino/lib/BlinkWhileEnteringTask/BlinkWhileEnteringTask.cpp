#include <BlinkWhileEnteringTask.hpp>

void blinkWhileEntering() {
    switch (context.blinkWhileEnteringState)
    {
    case LED2_OFF:
        if (context.carWashingSystemState == CAR_ENTERING){
            context.blinkWhileEnteringState = LED2_ON;
            context.led2->switchOn();
        }
        break;
    case LED2_ON:
        context.blinkWhileEnteringState = LED2_OFF;
        context.led2->switchOff();
        break;
    default:
        break;
    }
}

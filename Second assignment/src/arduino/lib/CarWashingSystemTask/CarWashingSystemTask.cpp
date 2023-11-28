#include <CarWashingSystemTask.hpp>
#include <avr/sleep.h>

int cnt1 = 0;
int cnt2 = 0;
int cnt3 = 0;
int cnt4 = 0;

String getEnumName(CarWashingSystemState state) {
    switch (state) {
    case EMPTY:
        return "EMPTY";
    case CHECK_IN:
        return "CHECK_IN";
    case CAR_ENTERING:
        return "CAR_ENTERING";
    case READY_TO_WASH:
        return "READY_TO_WASH";
    case WASHING:
        return "WASHING";
    case MAINTENANCE:
        return "MAINTENANCE";
    case CAR_LEAVING:
        return "CAR_LEAVING";
    default:
        return "UNKNOWN_STATE";
    }
}

void wakeUp() {}

void carWashingSystem() {
    switch (context.carWashingSystemState) {
    case EMPTY:
        if (context.carPresenceDetector->detectPresence()) {
            cnt1 = 0;
            context.led1->switchOn();
            context.userLCD->print(WELCOME_MSG);
            context.carWashingSystemState = CHECK_IN;
            context.pcDashboardComunicator->sendState(
                getEnumName(context.carWashingSystemState));
        } else {
            delay(500);
            attachInterrupt(digitalPinToInterrupt(PIR_PIN), &wakeUp, RISING);
            sleep_mode();
            detachInterrupt(digitalPinToInterrupt(PIR_PIN));
        }
        break;
    case CHECK_IN:
        if (cnt1 * CAR_WASHING_INTERVAL >= N1) {
            context.gate->open();
            cnt2 = 0;
            context.userLCD->print(PROCEED_MSG);
            context.carWashingSystemState = CAR_ENTERING;
            context.pcDashboardComunicator->sendState(
                getEnumName(context.carWashingSystemState));
        }
        cnt1++;
        break;
    case CAR_ENTERING:
        if (cnt2 * CAR_WASHING_INTERVAL >= N2) {
            context.gate->close();
            context.led2->switchOn();
            context.userLCD->print(READY_MSG);
            context.carWashingSystemState = READY_TO_WASH;
            context.pcDashboardComunicator->sendState(
                getEnumName(context.carWashingSystemState));
        }
        if (context.carDistanceDetector->detectDistance() <= MIN_DIST) {
            cnt2++;
        } else {
            cnt2 = 0;
        }
        break;
    case READY_TO_WASH:
        if (context.startActuator->isActive()) {
            cnt3 = 0;
            context.led2->switchOff();
            context.carWashingSystemState = WASHING;
            context.pcDashboardComunicator->sendState(
                getEnumName(context.carWashingSystemState));
            context.userLCD->startProgressBar(N3);
        }
        break;
    case WASHING:
        context.userLCD->tickProgressBar(cnt3 * CAR_WASHING_INTERVAL);
        if (context.inMaintenance) {
            context.userLCD->print(MAINTENANCE_MSG);
            context.carWashingSystemState = MAINTENANCE;
            context.pcDashboardComunicator->sendState(
                getEnumName(context.carWashingSystemState));
        } else if (cnt3 * CAR_WASHING_INTERVAL >= N3) {
            context.led2->switchOff();
            context.led3->switchOn();
            context.gate->open();
            cnt4 = 0;
            context.carWashingSystemState = CAR_LEAVING;
            context.userLCD->resetDisplay();
            context.pcDashboardComunicator->sendState(
                getEnumName(context.carWashingSystemState));
        }
        cnt3++;
        break;
    case MAINTENANCE:
        context.pcDashboardComunicator->sendTemperature(context.temperatureSensor->read());
        if (context.pcDashboardComunicator->isMaintenanceDone()) {
            context.inMaintenance = false;
            context.carWashingSystemState = WASHING;
            context.userLCD->restartProgressBar(cnt3 * CAR_WASHING_INTERVAL);
            context.pcDashboardComunicator->sendState(
                getEnumName(context.carWashingSystemState));
        }
        break;
    case CAR_LEAVING:
        if (cnt4 * CAR_WASHING_INTERVAL >= N4) {
            context.gate->close();
            context.led1->switchOff();
            context.led3->switchOff();
            context.carWashingSystemState = EMPTY;
            context.pcDashboardComunicator->sendNumberOfWashes(
                ++(context.numberOfWashes));
            context.pcDashboardComunicator->sendState(
                getEnumName(context.carWashingSystemState));
        }
        if (context.carDistanceDetector->detectDistance() >= MAX_DIST) {
            cnt4++;
        } else {
            cnt4 = 0;
        }
        break;
    default:
        break;
    }
}

#include <CarWashingSystemTask.hpp>
#include <avr/sleep.h>

#define INTERVAL 100

int cnt1 = 0;
int cnt2 = 0;
int cnt3 = 0;
int cnt4 = 0;
// il tick della task deve essere statico quindi non può essere una funzione della classe
// per avere comunque i dati del context si fa sto workaround
Context *taskContext;

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
    switch (taskContext->carWashingSystemState)
    {
    case EMPTY:
        if (taskContext->carPresenceDetector->detectPresence()) {
            cnt1 = 0;
            taskContext->led1->switchOn();
            taskContext->userLCD->print(WELCOME_MSG);
            taskContext->carWashingSystemState = CHECK_IN;
            taskContext->pcDashboardComunicator->sendState(getEnumName(taskContext->carWashingSystemState));
        } else {
            attachInterrupt(digitalPinToInterrupt(PIR_PIN), &wakeUp, RISING);
            sleep_mode();
            detachInterrupt(digitalPinToInterrupt(PIR_PIN));
        }
        break;
    case CHECK_IN:
        if (cnt1 * INTERVAL >= N1) {
            taskContext->gate->open();
            cnt2 = 0;
            taskContext->userLCD->print(PROCEED_MSG);
            taskContext->carWashingSystemState = CAR_ENTERING;
            taskContext->pcDashboardComunicator->sendState(getEnumName(taskContext->carWashingSystemState));
        }
        cnt1++;
        break;
    case CAR_ENTERING:
        if (cnt2 * INTERVAL >= N2) {
            taskContext->gate->close();
            taskContext->led2->switchOn();
            taskContext->userLCD->print(READY_MSG);
            taskContext->carWashingSystemState = READY_TO_WASH;
            taskContext->pcDashboardComunicator->sendState(getEnumName(taskContext->carWashingSystemState));
        }
        if (taskContext->carDistanceDetector->detectDistance() <= MIN_DIST) {
            cnt2++;
        } else {
            cnt2 = 0;
        }
        break;
    case READY_TO_WASH:
        if (taskContext->startActuator->isActive()) {
            cnt3 = 0;
            taskContext->led2->switchOff();
            taskContext->carWashingSystemState = WASHING;
            taskContext->pcDashboardComunicator->sendState(getEnumName(taskContext->carWashingSystemState));
            taskContext->userLCD->startProgressBar(N3);
        }
        break;
    case WASHING:
        taskContext->userLCD->tickProgressBar(cnt3 * INTERVAL);
        if (taskContext->inMaintenance) {
            taskContext->userLCD->print(MAINTENANCE_MSG);
            taskContext->carWashingSystemState = MAINTENANCE;
            taskContext->pcDashboardComunicator->sendState(getEnumName(taskContext->carWashingSystemState));
        } else if (cnt3 * INTERVAL >= N3) {
            taskContext->led2->switchOff();
            taskContext->led3->switchOn();
            taskContext->gate->open();
            cnt4 = 0;
            taskContext->carWashingSystemState = CAR_LEAVING;
            taskContext->userLCD->resetDisplay();
            taskContext->pcDashboardComunicator->sendState(getEnumName(taskContext->carWashingSystemState));
        }
        cnt3++;
        break;
    case MAINTENANCE:
        if (taskContext->pcDashboardComunicator->isMaintenanceDone())
        {
            taskContext->inMaintenance = false;
            taskContext->carWashingSystemState = WASHING;
            taskContext->userLCD->restartProgressBar(cnt3 * INTERVAL);
            taskContext->pcDashboardComunicator->sendState(getEnumName(taskContext->carWashingSystemState));
        }
        break;
    case CAR_LEAVING:
        if (cnt4 * INTERVAL >= N4) {
            taskContext->gate->close();
            taskContext->led1->switchOff();
            taskContext->led3->switchOff();
            taskContext->carWashingSystemState = EMPTY;
            taskContext->pcDashboardComunicator->sendNumberOfWashes(++(taskContext->numberOfWashes));
            taskContext->pcDashboardComunicator->sendState(getEnumName(taskContext->carWashingSystemState));
        }
        if (taskContext->carDistanceDetector->detectDistance() >= MAX_DIST) {
            cnt4++;
        } else {
            cnt4 = 0;
        }
        break;
    default:
        break;
    }
}

CarWashingSystemTask::CarWashingSystemTask(Context *context) : Task(INTERVAL, TASK_FOREVER, &carWashingSystem) {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    taskContext = context;
}

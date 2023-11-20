#include <Arduino.h>
#include <TaskScheduler.h>
#include <CarDistanceDetector.hpp>
#include <CarPresenceDetector.hpp>
#include <Light.h>
#include <Led.h>
#include <Gate.hpp>

#define MINDIST 50 // cm
#define MAXDIST 100 // cm
#define N1 2000 // ms
#define N2 2000 // ms
#define N3 2000 // ms
#define N4 2000 // ms

Scheduler scheduler;

enum SystemState {
    EMPTY,
    CHECK_IN,
    CAR_ENTERING,
    READY_TO_WASH,
    WASHING,
    MAINTENANCE,
    CAR_LEAVING
};
SystemState systemState = EMPTY;
CarDistanceDetector carDistanceDetector{11, 12};
CarPresenceDetector carPresenceDetector{10};
Light *led1 = new Led(9);
Light *led2 = new Led(8);
Light *led3 = new Led(7);
Gate gate{6};
int cnt1 = 0;
int cnt2 = 0;
int cnt3 = 0;
int cnt4 = 0;
bool inMaintenance = false;

void carWashingSystem() {
    switch (systemState)
    {
    case EMPTY:
        if (carPresenceDetector.detectPresence()) {
            // wake up
            cnt1 = 0;
            led1->switchOn(); // TODO non viene mai spento esplicitamente?
            // print on LCD "Welcome"
            systemState = CHECK_IN;
        } else {
            // go/stay in sleep
        }
        break;
    case CHECK_IN:
        if (cnt1 * carWashingSystemTask.getInterval() >= N1) {
            gate.open();
            cnt2 = 0;
            // print on LCD "Waiting"
            systemState = CAR_ENTERING;
        }
        cnt1++;
        break;
    case CAR_ENTERING:
        if (cnt2 * carWashingSystemTask.getInterval() >= N2) {
            gate.close();
            led2->switchOn();
            // print on LCD "Ready"
            systemState = READY_TO_WASH;
        }
        if (carDistanceDetector.detectDistance() <= MINDIST) {
            cnt2++;
        } else {
            cnt2 = 0;
        }
        break;
    case READY_TO_WASH:
        if (true /* usare oggetto button */) {
            cnt3 = 0;
            led2->switchOff();
            systemState = WASHING;
        }
        break;
    case WASHING:
        if (inMaintenance) {
            // stampa LCD "Maintenance"
            // stampa PC "Detected"
            systemState = MAINTENANCE;
        } else if (cnt3 * carWashingSystemTask.getInterval() >= N3) {
            led2->switchOff(); // TODO ha senso? non viene mai riacceso
            led3->switchOn();
            // stampa LCD "Washing"
            gate.open();
            cnt4 = 0;
            systemState = CAR_LEAVING;
        }
        cnt3++;
        // stampa LCD "Remaining"
        break;
    case MAINTENANCE:
        if (inMaintenance) {
            systemState = WASHING;
        }
        break;
    case CAR_LEAVING:
        if (cnt4 * carWashingSystemTask.getInterval() >= N4) {
            gate.close();
            led3->switchOn();
            systemState = EMPTY;
        }
        if (carDistanceDetector.detectDistance() >= MAXDIST) {
            cnt4++;
        } else {
            cnt4 = 0;
        }
        break;
    default:
        break;
    }
}

void blinkWhileWashing() {

}

void blinkWhileWaitingCar() {

}

void monitorTemperature() {

}

Task carWashingSystemTask(100, TASK_FOREVER, &carWashingSystem);
Task blinkWhileWashingTask(500, TASK_FOREVER, &blinkWhileWashing);
Task blinkWhileWaitingCarTask(100, TASK_FOREVER, &blinkWhileWaitingCar);
Task monitorTemperatureTask(100, TASK_FOREVER, &monitorTemperature);

void setup () {
    Serial.begin(115200);

    scheduler.init();
    scheduler.addTask(carWashingSystemTask);
    scheduler.addTask(blinkWhileWashingTask);
    scheduler.addTask(blinkWhileWaitingCarTask);
    scheduler.addTask(monitorTemperatureTask);

    carWashingSystemTask.enable();
    blinkWhileWashingTask.enable();
    blinkWhileWaitingCarTask.enable();
    monitorTemperatureTask.enable();
}

void loop () {
    scheduler.execute();
}
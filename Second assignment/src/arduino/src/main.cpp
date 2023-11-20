#include <Arduino.h>
#include <TaskScheduler.h>
#include <CarDistanceDetector.hpp>
#include <CarPresenceDetector.hpp>
#include <Light.h>
#include <Led.h>

#define N1 2000
#define N2 2000
#define N3 2000
#define N4 2000
#define N5 2000

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
int cnt1 = 0;
int cnt2 = 0;

void carWashingSystem() {
    switch (systemState)
    {
    case EMPTY:
        if (carPresenceDetector.detectPresence()) {
            // exit sleep
            cnt1 = 0;
            led1->switchOn();
            // print on LCD "Welcome"
            systemState = CHECK_IN;
        } else {
            // go/stay in sleep
        }
        break;
    case CHECK_IN:
        if (cnt1 * carWashingSystemTask.getInterval() >= N1) {
            // apri gate
            cnt2 = 0;
            // print on LCD "Waiting"
            systemState = CAR_ENTERING;
        }
        cnt1++;
        break;
    case CAR_ENTERING:
        break;
    case READY_TO_WASH:
        break;
    case WASHING:
        break;
    case MAINTENANCE:
        break;
    case CAR_LEAVING:
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
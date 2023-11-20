#include <Arduino.h>
#include <TaskScheduler.h>

#define PIR_PIN 10
#define L1_PIN 9
#define L2_PIN 8
#define L3_PIN 7

Scheduler scheduler;

enum SystemState {
    EMPTY,
    CHECK_IN,
    CAR_ENTERING,
    READY_TO_WASH,
    WASHING,
    MAINTENANCE,
    CAR_LEAVING
} systemState;
int cnt1 = 0;

void carWashingSystem() {
    switch (systemState)
    {
    case EMPTY:
        if (digitalRead(PIR_PIN) == HIGH) {
            // exit sleep
            digitalWrite(L1_PIN, HIGH);
            // print on LCD "Welcome"
            systemState = CHECK_IN;
        } else {
            // go/stay in sleep
        }
        break;
    case CHECK_IN:
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
    systemState = EMPTY;

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
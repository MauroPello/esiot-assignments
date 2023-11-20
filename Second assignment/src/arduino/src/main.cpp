#include <Arduino.h>
#include <TaskScheduler.h>
#include <CarDistanceDetector.hpp>
#include <CarPresenceDetector.hpp>
#include <Light.hpp>
#include <Led.hpp>
#include <Gate.hpp>
#include <avr/sleep.h>

#define SONAR_TRIG_PIN 12
#define SONAR_ECHO_PIN 11
#define PIR_PIN 10
#define LED1_PIN 9
#define LED2_PIN 8
#define LED3_PIN 7
#define MINDIST 50 // cm
#define MAXDIST 100 // cm
#define N1 2000 // ms
#define N2 2000 // ms
#define N3 2000 // ms
#define N4 2000 // ms

Scheduler scheduler;

enum CarWashingSystemState {
    EMPTY,
    CHECK_IN,
    CAR_ENTERING,
    READY_TO_WASH,
    WASHING,
    MAINTENANCE,
    CAR_LEAVING
};
CarWashingSystemState carWashingSystemState = EMPTY;
CarDistanceDetector carDistanceDetector{SONAR_TRIG_PIN, SONAR_ECHO_PIN};
CarPresenceDetector carPresenceDetector{PIR_PIN};
Light *led1 = new Led(LED1_PIN);
Light *led2 = new Led(LED2_PIN);
Light *led3 = new Led(LED3_PIN);
Gate gate{6};
int cnt1 = 0;
int cnt2 = 0;
int cnt3 = 0;
int cnt4 = 0;
bool inMaintenance = false;

void carWashingSystem() {
    switch (carWashingSystemState)
    {
    case EMPTY:
        if (carPresenceDetector.detectPresence()) {
            cnt1 = 0;
            led1->switchOn();
            // print on LCD "Welcome"
            carWashingSystemState = CHECK_IN;
        } else {
            sleep_enable();
        }
        break;
    case CHECK_IN:
        if (cnt1 * carWashingSystemTask.getInterval() >= N1) {
            gate.open();
            cnt2 = 0;
            // print on LCD "Waiting"
            carWashingSystemState = CAR_ENTERING;
        }
        cnt1++;
        break;
    case CAR_ENTERING:
        if (cnt2 * carWashingSystemTask.getInterval() >= N2) {
            gate.close();
            led2->switchOn();
            // print on LCD "Ready"
            carWashingSystemState = READY_TO_WASH;
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
            carWashingSystemState = WASHING;
        }
        break;
    case WASHING:
        if (inMaintenance) {
            // print on LCD "Maintenance"
            // print on PC "Detected"
            carWashingSystemState = MAINTENANCE;
        } else if (cnt3 * carWashingSystemTask.getInterval() >= N3) {
            led2->switchOff();
            led3->switchOn();
            // print on LCD "Washing"
            gate.open();
            cnt4 = 0;
            carWashingSystemState = CAR_LEAVING;
        }
        cnt3++;
        // print on LCD "Remaining"
        break;
    case MAINTENANCE:
        if (inMaintenance) {
            carWashingSystemState = WASHING;
        }
        break;
    case CAR_LEAVING:
        if (cnt4 * carWashingSystemTask.getInterval() >= N4) {
            gate.close();
            led1->switchOff();
            led3->switchOff();
            carWashingSystemState = EMPTY;
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

enum BlinkLed2State {
    LED2_OFF,
    LED2_ON
};
BlinkLed2State blinkWhileWashingState = LED2_OFF;

void blinkWhileWashing() {
    switch (blinkWhileWashingState)
    {
    case LED2_OFF:
        break;
    case LED2_ON:
        break;
    default:
        break;
    }
}

BlinkLed2State blinkWhileWaitingCarState = LED2_OFF;

void blinkWhileWaitingCar() {
    switch (blinkWhileWaitingCarState)
    {
    case LED2_OFF:
        break;
    case LED2_ON:
        break;
    default:
        break;
    }
}

enum MonitorTemperatureState {
    SLEEPING,
    ACTIVE
};
MonitorTemperatureState monitorTemperatureState = SLEEPING;

void monitorTemperature() {
    switch (monitorTemperatureState)
    {
    case SLEEPING:
        break;
    case ACTIVE:
        break;
    default:
        break;
    }
}

Task carWashingSystemTask(100, TASK_FOREVER, &carWashingSystem);
Task blinkWhileWashingTask(500, TASK_FOREVER, &blinkWhileWashing);
Task blinkWhileWaitingCarTask(100, TASK_FOREVER, &blinkWhileWaitingCar);
Task monitorTemperatureTask(100, TASK_FOREVER, &monitorTemperature);

void setup () {
    Serial.begin(9600);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    attachInterrupt(digitalPinToInterrupt(PIR_PIN), NULL, HIGH);

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
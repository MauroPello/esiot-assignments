#include <Arduino.h>
#include <TaskScheduler.h>
#include <CarDistanceDetector.hpp>
#include <CarPresenceDetector.hpp>
#include <Light.hpp>
#include <Led.hpp>
#include <StartActuator.hpp>
#include <Gate.hpp>
#include <UserLCD.hpp>
#include <avr/sleep.h>
#include <TemperatureSensor.hpp>
#include <PCDashboardComunicator.hpp>

#define SONAR_TRIG_PIN 12
#define SONAR_ECHO_PIN 11
#define PIR_PIN 10
#define LED1_PIN 9
#define LED2_PIN 8
#define LED3_PIN 7
#define BUTTON_PIN 6
#define GATE_PIN 5
#define TEMP_PIN A0
#define MINDIST 50 // cm
#define MAXDIST 10 // cm
#define N1 2000 // ms
#define N2 2000 // ms
#define N3 10000 // ms
#define N4 2000 // ms
#define MAX_TEMP 85 // °C
#define WELCOME_MSG "Welcome"
#define PROCEED_MSG "Proceed to the Washing Area"
#define READY_MSG "Ready to Wash"
#define WASHING_COMPLETE_MSG "Washing complete, you can leave the area"
#define MAINTENANCE_MSG "Detected a Problem - Please Wait"

void carWashingSystem();
void blinkWhileWashing();
void blinkWhileEntering();
void monitorTemperature();

Scheduler scheduler;
Task carWashingSystemTask(100, TASK_FOREVER, &carWashingSystem);
Task blinkWhileWashingTask(500, TASK_FOREVER, &blinkWhileWashing);
Task blinkWhileEnteringTask(100, TASK_FOREVER, &blinkWhileEntering);
Task monitorTemperatureTask(100, TASK_FOREVER, &monitorTemperature);

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
CarDistanceDetector *carDistanceDetector;
CarPresenceDetector *carPresenceDetector;
Light *led1;
Light *led2;
Light *led3;
StartActuator *startActuator;
UserLCD *userLCD;
Gate *gate;
PCDashboardComunicator *pcDashboardComunicator;
TemperatureSensor *temperatureSensor;
int cnt1 = 0;
int cnt2 = 0;
int cnt3 = 0;
int cnt4 = 0;
int cnt5 = 0;
int numberOfWashes = 0;
bool inMaintenance = false;

//TODO: Utils??
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

void carWashingSystem() {
    switch (carWashingSystemState)
    {
    case EMPTY:
        if (carPresenceDetector->detectPresence()) {
            cnt1 = 0;
            led1->switchOn();
            userLCD->print(WELCOME_MSG);
            carWashingSystemState = CHECK_IN;
            pcDashboardComunicator->sendState(getEnumName(carWashingSystemState));
        } else {
            attachInterrupt(digitalPinToInterrupt(PIR_PIN), NULL, HIGH);
            sleep_enable();
            detachInterrupt(digitalPinToInterrupt(PIR_PIN));
        }
        break;
    case CHECK_IN:
        if (cnt1 * carWashingSystemTask.getInterval() >= N1) {
            gate->open();
            cnt2 = 0;
            userLCD->print(PROCEED_MSG);
            carWashingSystemState = CAR_ENTERING;
            pcDashboardComunicator->sendState(getEnumName(carWashingSystemState));
        }
        cnt1++;
        break;
    case CAR_ENTERING:
        if (cnt2 * carWashingSystemTask.getInterval() >= N2) {
            gate->close();
            led2->switchOn();
            userLCD->print(READY_MSG);
            carWashingSystemState = READY_TO_WASH;
            pcDashboardComunicator->sendState(getEnumName(carWashingSystemState));
        }
        if (carDistanceDetector->detectDistance() <= MINDIST) {
            cnt2++;
        } else {
            cnt2 = 0;
        }
        break;
    case READY_TO_WASH:
        if (startActuator->isActive()) {
            cnt3 = 0;
            led2->switchOff();
            carWashingSystemState = WASHING;
            pcDashboardComunicator->sendState(getEnumName(carWashingSystemState));
            userLCD->startProgressBar(N3);
        }
        break;
    case WASHING:
        userLCD->tickProgressBar(cnt3 * carWashingSystemTask.getInterval());
        if (inMaintenance) {
            userLCD->print(MAINTENANCE_MSG);
            carWashingSystemState = MAINTENANCE;
            pcDashboardComunicator->sendState(getEnumName(carWashingSystemState));
        } else if (cnt3 * carWashingSystemTask.getInterval() >= N3) {
            led2->switchOff();
            led3->switchOn();
            gate->open();
            cnt4 = 0;
            carWashingSystemState = CAR_LEAVING;
            userLCD->resetDisplay();
            pcDashboardComunicator->sendState(getEnumName(carWashingSystemState));
        }
        cnt3++;
        break;
    case MAINTENANCE:
        if (pcDashboardComunicator->isMaintenanceDone())
        {
            inMaintenance = false;
            carWashingSystemState = WASHING;
            userLCD->restartProgressBar(cnt3);
            pcDashboardComunicator->sendState(getEnumName(carWashingSystemState));
        }
        break;
    case CAR_LEAVING:
        if (cnt4 * carWashingSystemTask.getInterval() >= N4) {
            gate->close();
            led1->switchOff();
            led3->switchOff();
            carWashingSystemState = EMPTY;
            pcDashboardComunicator->sendNumberOfWashes(++numberOfWashes);
            pcDashboardComunicator->sendState(getEnumName(carWashingSystemState));
        }
        if (carDistanceDetector->detectDistance() >= MAXDIST) {
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
        if (carWashingSystemState == WASHING) {
            blinkWhileWashingState = LED2_ON;
            led2->switchOn();
        }
        break;
    case LED2_ON:
        blinkWhileWashingState = LED2_OFF;
        led2->switchOff();
        break;
    default:
        break;
    }
}

BlinkLed2State blinkWhileEnteringState = LED2_OFF;

void blinkWhileEntering() {
    switch (blinkWhileEnteringState)
    {
    case LED2_OFF:
        if (carWashingSystemState == CAR_ENTERING){
            blinkWhileEnteringState = LED2_ON;
            led2->switchOn();
        }
        break;
    case LED2_ON:
        blinkWhileEnteringState = LED2_OFF;
        led2->switchOff();
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
        if (carWashingSystemState == WASHING) {
            monitorTemperatureState = ACTIVE;
        }
        break;
    case ACTIVE:{
        if (carWashingSystemState != WASHING) {
            monitorTemperatureState = SLEEPING;
        }
        float temp = temperatureSensor->read();
        pcDashboardComunicator->sendTemperature(temp);
        if (temp < MAX_TEMP) {
            cnt5 = 0;
        } else {
            cnt5++;
        }
        if (cnt5 * monitorTemperatureTask.getInterval() >= N4) {
            inMaintenance = true;
        }
        break;
    }
    default:
        break;
    }
}

void setup () {
    Serial.begin(9600);
    carDistanceDetector = new CarDistanceDetector(SONAR_TRIG_PIN, SONAR_ECHO_PIN);
    carPresenceDetector = new CarPresenceDetector(PIR_PIN);
    led1 = new Led(LED1_PIN);
    led2 = new Led(LED2_PIN);
    led3 = new Led(LED3_PIN);
    startActuator = new StartActuator(BUTTON_PIN);
    userLCD = new UserLCD();
    gate = new Gate(GATE_PIN);
    pcDashboardComunicator = new PCDashboardComunicator();
    temperatureSensor = new TemperatureSensor(TEMP_PIN);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    scheduler.init();
    scheduler.addTask(carWashingSystemTask);
    scheduler.addTask(blinkWhileWashingTask);
    scheduler.addTask(blinkWhileEnteringTask);
    scheduler.addTask(monitorTemperatureTask);

    carWashingSystemTask.enable();
    blinkWhileWashingTask.enable();
    blinkWhileEnteringTask.enable();
    monitorTemperatureTask.enable();
}

void loop () {
    scheduler.execute();
}
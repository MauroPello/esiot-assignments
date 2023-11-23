#ifndef CONTEXT_HPP
#define CONTEXT_HPP
#include <Arduino.h>
#include <CarDistanceDetector.hpp>
#include <CarPresenceDetector.hpp>
#include <Light.hpp>
#include <Led.hpp>
#include <StartActuator.hpp>
#include <Gate.hpp>
#include <UserLCD.hpp>
#include <TemperatureSensor.hpp>
#include <PCDashboardComunicator.hpp>
#include <TaskSchedulerDeclarations.h>

#define SONAR_TRIG_PIN 12
#define SONAR_ECHO_PIN 11
#define PIR_PIN 3
#define LED1_PIN 10
#define LED2_PIN 9
#define LED3_PIN 8
#define BUTTON_PIN 7
#define GATE_PIN 6
#define TEMP_PIN A0
#define MIN_DIST 30 // cm
#define MAX_DIST 50 // cm
#define MAX_TEMP 40 // °C
#define N1 2000 // ms
#define N2 1000 // ms
#define N3 5000 // ms
#define N4 N2 // ms
#define WELCOME_MSG "Welcome"
#define PROCEED_MSG "Proceed to the Washing Area"
#define READY_MSG "Ready to Wash"
#define WASHING_COMPLETE_MSG "Washing complete, you can leave the area"
#define MAINTENANCE_MSG "Detected a Problem - Please Wait"

enum CarWashingSystemState {
    EMPTY,
    CHECK_IN,
    CAR_ENTERING,
    READY_TO_WASH,
    WASHING,
    MAINTENANCE,
    CAR_LEAVING
};

enum BlinkLed2State {
    LED2_OFF,
    LED2_ON
};

enum MonitorTemperatureState {
    SLEEPING,
    ACTIVE
};

extern struct Context {
    CarWashingSystemState carWashingSystemState;
    BlinkLed2State blinkWhileWashingState;
    BlinkLed2State blinkWhileEnteringState;
    MonitorTemperatureState monitorTemperatureState;
    int numberOfWashes;
    bool inMaintenance;
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
} context;


#endif
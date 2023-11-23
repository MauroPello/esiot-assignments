#include <Context.hpp>
#include <CarWashingSystemTask.hpp>
#include <TaskScheduler.h>

void blinkWhileWashing();
void blinkWhileEntering();
void monitorTemperature();

extern Context context;
Scheduler scheduler;
CarWashingSystemTask *carWashingSystemTask;
Task blinkWhileWashingTask(500, TASK_FOREVER, &blinkWhileWashing);
Task blinkWhileEnteringTask(100, TASK_FOREVER, &blinkWhileEntering);
Task monitorTemperatureTask(100, TASK_FOREVER, &monitorTemperature);

void blinkWhileWashing() {
    switch (context.blinkWhileWashingState)
    {
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

int cnt5 = 0;

void monitorTemperature() {
    switch (context.monitorTemperatureState)
    {
    case SLEEPING:
        if (context.carWashingSystemState == WASHING) {
            context.monitorTemperatureState = ACTIVE;
        }
        break;
    case ACTIVE:{
        if (context.carWashingSystemState != WASHING) {
            context.monitorTemperatureState = SLEEPING;
        }
        float temp = context.temperatureSensor->read();
        context.pcDashboardComunicator->sendTemperature(temp);
        if (temp < MAX_TEMP) {
            cnt5 = 0;
        } else {
            cnt5++;
        }
        if (cnt5 * monitorTemperatureTask.getInterval() >= N4) {
            context.inMaintenance = true;
        }
        break;
    }
    default:
        break;
    }
}

void setup () {
    Serial.begin(9600);
    context.carDistanceDetector = new CarDistanceDetector(SONAR_TRIG_PIN, SONAR_ECHO_PIN);
    context.carPresenceDetector = new CarPresenceDetector(PIR_PIN);
    context.led1 = new Led(LED1_PIN);
    context.led2 = new Led(LED2_PIN);
    context.led3 = new Led(LED3_PIN);
    context.startActuator = new StartActuator(BUTTON_PIN);
    context.userLCD = new UserLCD();
    context.gate = new Gate(GATE_PIN);
    context.pcDashboardComunicator = new PCDashboardComunicator();
    context.temperatureSensor = new TemperatureSensor(TEMP_PIN);
    context.carWashingSystemState = EMPTY;
    context.blinkWhileWashingState = LED2_OFF;
    context.blinkWhileEnteringState = LED2_OFF;
    context.monitorTemperatureState = SLEEPING;
    context.numberOfWashes = 0;
    context.inMaintenance = false;

    carWashingSystemTask = new CarWashingSystemTask();

    scheduler.init();
    scheduler.addTask(*carWashingSystemTask);
    scheduler.addTask(blinkWhileWashingTask);
    scheduler.addTask(blinkWhileEnteringTask);
    scheduler.addTask(monitorTemperatureTask);

    carWashingSystemTask->enable();
    blinkWhileWashingTask.enable();
    blinkWhileEnteringTask.enable();
    monitorTemperatureTask.enable();
}

void loop () {
    scheduler.execute();
}
#include <Context.hpp>
#include <avr/sleep.h>
#include <TaskScheduler.h>
#include <CarWashingSystemTask.hpp>
#include <BlinkWhileWashingTask.hpp>
#include <BlinkWhileEnteringTask.hpp>
#include <MonitorTemperatureTask.hpp>

Context context{};

Scheduler scheduler;
Task carWashingSystemTask(CAR_WASHING_INTERVAL, TASK_FOREVER,
                          &carWashingSystem);
Task blinkWhileWashingTask(BLINK_WHILE_WASHING_INTERVAL, TASK_FOREVER,
                           &blinkWhileWashing);
Task blinkWhileEnteringTask(BLINK_WHILE_ENTERING_INTERVAL, TASK_FOREVER,
                            &blinkWhileEntering);
Task monitorTemperatureTask(MONITOR_TEMPERATURE_INTERVAL, TASK_FOREVER,
                            &monitorTemperature);

void setup() {
    Serial.begin(9600);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    context.carDistanceDetector =
        new CarDistanceDetector(SONAR_TRIG_PIN, SONAR_ECHO_PIN);
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

void loop() {
    scheduler.execute();
}
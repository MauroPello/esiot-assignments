#include <MonitorTemperatureTask.hpp>

int cnt5 = 0;

void monitorTemperature() {
    switch (context.monitorTemperatureState) {
    case SLEEPING:
        if (context.carWashingSystemState == WASHING) {
            context.monitorTemperatureState = ACTIVE;
            cnt5 = 0;
        }
        break;
    case ACTIVE: {
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
        if (cnt5 * MONITOR_TEMPERATURE_INTERVAL >= N4) {
            context.inMaintenance = true;
        }
        break;
    }
    default:
        break;
    }
}

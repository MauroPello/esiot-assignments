#ifndef PC_DASHBOARD_COMUNICATOR_HPP
#define PC_DASHBOARD_COMUNICATOR_HPP
#include <Arduino.h>

class PCDashboardComunicator{
private:
    String readSerialString();
    void sendMessage(String prefix, String value);
public:
    PCDashboardComunicator();
    void sendTemperature(float temp);
    void sendState(String State);
    void sendNumberOfWashes(int n);
    bool isMaintenanceDone();
};

#endif
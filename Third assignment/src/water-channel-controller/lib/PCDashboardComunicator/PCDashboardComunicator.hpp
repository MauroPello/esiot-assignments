#ifndef PC_DASHBOARD_COMUNICATOR_HPP
#define PC_DASHBOARD_COMUNICATOR_HPP
#include <Arduino.h>

class PCDashboardComunicator {
private:
	void sendMessage(String prefix, String value);

public:
	PCDashboardComunicator();
	void sendStateAndLevel(String State, int percentage);
	int getValveLevel();
};

#endif
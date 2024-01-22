#ifndef PC_DASHBOARD_COMUNICATOR_HPP
#define PC_DASHBOARD_COMUNICATOR_HPP
#include <Arduino.h>

class PCDashboardComunicator {
private:
	void sendMessage(String prefix, String value);

public:
	PCDashboardComunicator();
	void sendValveLevel(int percentage);
	void sendState(String State);
	int getValveLevel();
};

#endif
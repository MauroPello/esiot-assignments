#include <Arduino.h>
#include "Display.hpp"
#include "ManualModeActivator.hpp"
#include "ManualModeController.hpp"
#include "Valve.hpp"
#include "PCDashboardComunicator.hpp"

enum SystemState
{
	MANUAL,
	AUTOMATIC
};

SystemState systemState;

Display *display;
ManualModeActivator *manualModeActivator;
Valve *valve;
ManualModeController *manualModeController;
PCDashboardComunicator *dashboardComunicator;
int currentAutomaticValveLevel = 0;
int currentManualValveLevel = 0;
bool stateChanged = false;

void buttonPressed()
{
	static unsigned long last_interrupt_time = 0;
	unsigned long interrupt_time = millis();
	if (interrupt_time - last_interrupt_time > 200)
	{
		if (systemState == MANUAL)
		{
			systemState = AUTOMATIC;
		}
		else
		{
			systemState = MANUAL;
		}
		stateChanged = true;
	}
	last_interrupt_time = interrupt_time;
}

void setup()
{
	display = new Display();
	manualModeActivator = new ManualModeActivator(2, buttonPressed);
	manualModeController = new ManualModeController(A0);
	valve = new Valve(9);
	currentAutomaticValveLevel = 0;
	currentManualValveLevel = 0;
	Serial.begin(9600);
	stateChanged = true;
	systemState = AUTOMATIC;
}

void loop()
{
	int valveLevel;
	switch (systemState)
	{
	case MANUAL:
		valveLevel = manualModeController->getValveLevel();
		if (currentManualValveLevel != valveLevel)
		{
			dashboardComunicator->sendStateAndLevel("MANUAL", valveLevel);
			valve->setLevel(valveLevel);
			stateChanged = true;
			currentManualValveLevel = valveLevel;
		}
		break;
	case AUTOMATIC:
		valveLevel = dashboardComunicator->getValveLevel();
		if (currentAutomaticValveLevel != valveLevel && valveLevel != -1)
		{
			dashboardComunicator->sendStateAndLevel("AUTOMATIC", valveLevel);
			valve->setLevel(valveLevel);
			stateChanged = true;
			currentAutomaticValveLevel = valveLevel;
		}
		break;
	default:
		break;
	}

	if (stateChanged)
	{
		stateChanged = false;
		int currentValveLevel = systemState == MANUAL ? currentManualValveLevel : currentAutomaticValveLevel;
		valve->setLevel(currentValveLevel);
		display->print(systemState == MANUAL ? "MANUAL" : "AUTOMATIC", currentValveLevel);
	}
}

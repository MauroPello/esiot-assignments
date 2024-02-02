#include <Arduino.h>
#include "Display.hpp"
#include "ManualModeActivator.hpp"
#include "ManualModeController.hpp"
#include "Valve.hpp"
#include "PCDashboardComunicator.hpp"

#define MANUAL_MODE_ACTIVATOR_PIN 2
#define MANUAL_MODE_CONTROLLER_PIN A0
#define VALVE_PIN 9

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
int currentAutomaticValveLevel = 25;
int currentManualValveLevel = 25;
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
	manualModeActivator = new ManualModeActivator(MANUAL_MODE_ACTIVATOR_PIN, buttonPressed);
	manualModeController = new ManualModeController(MANUAL_MODE_CONTROLLER_PIN);
	valve = new Valve(VALVE_PIN);
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
			stateChanged = true;
			currentManualValveLevel = valveLevel;
		}
		break;
	case AUTOMATIC:
		valveLevel = dashboardComunicator->getValveLevel();
		if (currentAutomaticValveLevel != valveLevel && valveLevel != -1)
		{
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
		String currentState = systemState == MANUAL ? "MANUAL" : "AUTOMATIC";
		dashboardComunicator->sendStateAndLevel(currentState, currentValveLevel);
		display->print(currentState, currentValveLevel);
	}
}

#include <Arduino.h>
#include "Display.hpp"
#include "ManualModeActivator.hpp"
#include "ManualModeController.hpp"
#include "Valve.hpp"

enum SystemState{
  MANUAL,
  AUTOMATIC
} systemState;

Display *display;
ManualModeActivator *manualModeActivator;
Valve *valve;
ManualModeController *manualModeController;
int currentValveLevel = 0;
bool stateChanged = false;

void buttonPressed(){
	static unsigned long last_interrupt_time = 0;
	unsigned long interrupt_time = millis();
	if (interrupt_time - last_interrupt_time > 200)
	{
		if (systemState == MANUAL) {
			systemState = AUTOMATIC;
		} else {
			systemState = MANUAL;
		}
		stateChanged = true;
	}
	last_interrupt_time = interrupt_time;
}

void setup() {
	display = new Display();
	manualModeActivator = new ManualModeActivator(2, buttonPressed);
	manualModeController = new ManualModeController(A0);
	valve = new Valve(9); // TODO dire a valve livello di partenza.
	currentValveLevel = 0;
	Serial.begin(9600);
	stateChanged = true;
}

void loop() {
	switch(systemState) {
		case MANUAL:
			int valveLevel = manualModeController->getValveLevel();
			if (currentValveLevel != valveLevel) {
				valve->setLevel(valveLevel);
				stateChanged = true;
				currentValveLevel = valveLevel;
			}
			break;
		case AUTOMATIC:
			break;
	}

	if(stateChanged){
		stateChanged = false;
		display->print(systemState == MANUAL ? "MANUAL" : "AUTOMATIC", currentValveLevel);
	}
}

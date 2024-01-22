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
	}
	last_interrupt_time = interrupt_time;
}

void setup() {
	display = new Display();
	manualModeActivator = new ManualModeActivator(2, buttonPressed);
	manualModeController = new ManualModeController(A0);
	valve = new Valve(9);
	Serial.begin(9600);
}

void loop() {

	switch(systemState) {
		case MANUAL:
			break;
		case AUTOMATIC:
			break;

	}
}

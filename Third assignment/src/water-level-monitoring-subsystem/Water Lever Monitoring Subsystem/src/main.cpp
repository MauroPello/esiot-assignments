#include <Arduino.h>
#include <CommunicationSystem.hpp>
#include <Led.hpp>
#include <WaterLevelDetector.hpp>
#define GREEN_LED_PIN 2 //TODO metterli sensati
#define RED_LED_PIN 2
#define TRIG_SONAR_PIN 2
#define ECHO_SONAR_PIN 2

Led *greenLed;
Led *redLed;
WaterLevelDetector *waterLevelDetector;
CommunicationSystem *communicationSystem;

void setup()
{
	greenLed = new Led(GREEN_LED_PIN);
	redLed = new Led(RED_LED_PIN);
	communicationSystem = new CommunicationSystem();
	waterLevelDetector = new WaterLevelDetector(TRIG_SONAR_PIN, ECHO_SONAR_PIN);
	Serial.begin(9600);
}

void loop()
{
	if (!communicationSystem->isConnected())
	{
		greenLed->switchOff();
		redLed->switchOn();
		communicationSystem->reconnect();
		greenLed->switchOn();
		redLed->switchOff();
	}

	unsigned long now = millis();
	// fai enum con F1 e F2
	if (now > 10000)
	{
		// capire questione ricezione e invio
	}
}
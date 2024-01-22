#include <Arduino.h>
#include <CommunicationSystem.hpp>
#include <Led.hpp>
#include <WaterLevelDetector.hpp>

#define GREEN_LED_PIN 2 // TODO metterli sensati
#define RED_LED_PIN 2
#define TRIG_SONAR_PIN 2
#define ECHO_SONAR_PIN 2
#define F1 2000
#define F2 1000

Led *greenLed;
Led *redLed;
WaterLevelDetector *waterLevelDetector;
CommunicationSystem *communicationSystem;
int currentFrequency;
long lastMsgTime = 0;

void setup()
{
	greenLed = new Led(GREEN_LED_PIN);
	redLed = new Led(RED_LED_PIN);
	communicationSystem = new CommunicationSystem();
	waterLevelDetector = new WaterLevelDetector(TRIG_SONAR_PIN, ECHO_SONAR_PIN);
	currentFrequency = F1;
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
	else
	{
		String msg = communicationSystem->receiveData();

		if (msg = "alarm")
		{
			currentFrequency = F2;
		}
		else if (msg = "normal")
		{
			currentFrequency = F1;
		}
	}

	unsigned long now = millis();
	if (now - lastMsgTime > currentFrequency)
	{
		int waterLeverMeasure = waterLevelDetector->detectLevel();
		communicationSystem->sendData((char *)("WL:" + waterLeverMeasure));
	}
}
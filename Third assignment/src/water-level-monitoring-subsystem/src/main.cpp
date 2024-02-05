#include <Arduino.h>
#include <CommunicationSystem.hpp>
#include <Led.hpp>
#include <WaterLevelDetector.hpp>

#define GREEN_LED_PIN 4
#define RED_LED_PIN 2
#define TRIG_SONAR_PIN 5
#define ECHO_SONAR_PIN 18

Led *greenLed;
Led *redLed;
WaterLevelDetector *waterLevelDetector;
CommunicationSystem *communicationSystem;
int currentFrequency = 1000;
long lastMsgTime = 0;

void setup()
{
	Serial.begin(9600);
	greenLed = new Led(GREEN_LED_PIN);
	redLed = new Led(RED_LED_PIN);
	redLed->switchOn();
	communicationSystem = new CommunicationSystem();
	redLed->switchOff();
	waterLevelDetector = new WaterLevelDetector(TRIG_SONAR_PIN, ECHO_SONAR_PIN);
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
		if(msg != "") {
			msg.remove(0, 2);
			msg.trim();
      		currentFrequency = msg.toInt();
			Serial.println("Valore frequenza: ");
			Serial.println(currentFrequency);
			Serial.println("Valore stringa:" + msg + ".");
		}
	}

	unsigned long now = millis();
	if (now - lastMsgTime > currentFrequency)
	{
		int waterLevelMeasure = waterLevelDetector->detectLevel();
		Serial.println("Distanza misurata: " + String(waterLevelMeasure));
		communicationSystem->sendData("WL:" + String(waterLevelMeasure));
		lastMsgTime = now;
	}
}
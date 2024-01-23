#include <Arduino.h>
#include <WaterLevelDetector.hpp>

WaterLevelDetector::WaterLevelDetector(int trigPin, int echoPin)
    : trigPin{trigPin}, echoPin{echoPin} {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
};

int WaterLevelDetector::detectLevel() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    return MAX_DISTANCE - pulseIn(echoPin, HIGH) * 0.034 / 2;
}

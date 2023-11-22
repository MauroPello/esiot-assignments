#include <Arduino.h>
#include <CarDistanceDetector.hpp>

CarDistanceDetector::CarDistanceDetector(int trigPin, int echoPin) : trigPin{trigPin}, echoPin{echoPin} {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
};

int CarDistanceDetector::detectDistance() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    return pulseIn(echoPin, HIGH) * 0.034 / 2;
}

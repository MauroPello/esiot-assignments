#include <CarDistanceDetector.hpp>

Sonar::Sonar(int trigPin, int echoPin) : trigPin{trigPin}, echoPin{echoPin} {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
};

bool Sonar::detectDistance() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    int distance = pulseIn(echoPin, HIGH) * 0.034 / 2;
    return distance;
}

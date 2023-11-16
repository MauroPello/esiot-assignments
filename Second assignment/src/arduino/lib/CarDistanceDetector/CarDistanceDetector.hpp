#ifndef SONAR_HPP
#define SONAR_HPP
#include <Arduino.h>

class Sonar {
    const int trigPin;
    const int echoPin;
    public:
        Sonar(int trigPin, int echoPin);
        bool detectDistance();
};

#endif
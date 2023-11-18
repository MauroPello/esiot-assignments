#ifndef CAR_DISTANCE_DETECTOR_HPP
#define CAR_DISTANCE_DETECTOR_HPP
#include <Arduino.h>

class CarDistanceDetector {
    const int trigPin;
    const int echoPin;
    public:
        CarDistanceDetector(int trigPin, int echoPin);
        bool detectDistance();
};

#endif
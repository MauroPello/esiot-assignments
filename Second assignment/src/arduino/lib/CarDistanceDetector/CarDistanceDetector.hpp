#ifndef CAR_DISTANCE_DETECTOR_HPP
#define CAR_DISTANCE_DETECTOR_HPP

class CarDistanceDetector {
  private:
    const int trigPin;
    const int echoPin;

  public:
    CarDistanceDetector(int trigPin, int echoPin);
    int detectDistance();
};

#endif
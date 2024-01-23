#ifndef WATER_LEVEL_DETECTOR_HPP
#define WATER_LEVEL_DETECTOR_HPP
#define MAX_DISTANCE 25


class WaterLevelDetector {
  private:
    const int trigPin;
    const int echoPin;

  public:
    WaterLevelDetector(int trigPin, int echoPin);
    int detectLevel();
};

#endif
#ifndef WATER_LEVEL_DETECTOR_HPP
#define WATER_LEVEL_DETECTOR_HPP

class WaterLevelDetector {
  private:
    const int trigPin;
    const int echoPin;

  public:
    WaterLevelDetector(int trigPin, int echoPin);
    int detectLevel();
};

#endif
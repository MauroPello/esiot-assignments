#ifndef CAR_PRESENCE_DETECTOR_HPP
#define CAR_PRESENCE_DETECTOR_HPP

class CarPresenceDetector {
  private:
    const int outPin;

  public:
    CarPresenceDetector(int outPin);
    bool detectPresence();
};

#endif
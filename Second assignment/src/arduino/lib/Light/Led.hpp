#ifndef LED_HPP
#define LED_HPP
#include <Light.hpp>

class Led : public Light {
  private:
    const int pin;

  public:
    Led(int pin);
    void switchOn();
    void switchOff();
};

#endif
#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class Display {
  private:
    LiquidCrystal_I2C lcd;

  public:
    Display();
    void print(String mode, int percentage);
};

#endif
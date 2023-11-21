#ifndef USER_LCD_HPP
#define USER_LCD_HPP
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class UserLCD {
    LiquidCrystal_I2C lcd;
    void resetCursor();
public:
    UserLCD();
    void print(String msg);
    void drawProgressBar(int progress, int max);
};

#endif
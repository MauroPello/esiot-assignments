#ifndef USER_LCD_HPP
#define USER_LCD_HPP
#include <LiquidCrystal_I2C.h>

class UserLCD {
public:
    UserLCD();
    void tick();
    void startCounter();
    void print(char* msg);
private:
    LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);
    int counter = 0;
};

#endif
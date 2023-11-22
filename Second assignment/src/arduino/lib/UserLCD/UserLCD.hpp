#ifndef USER_LCD_HPP
#define USER_LCD_HPP
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class UserLCD {
    private:
        LiquidCrystal_I2C lcd;
        byte barFragment[8];
        int maxProgress;
        void printColumnProgress(int colIndex);
    public:
        UserLCD();
        void print(String msg);
        void startProgressBar(int max);
        void tickProgressBar(int progress);
        void resetDisplay();
        void restartProgressBar(int progress);
};

#endif
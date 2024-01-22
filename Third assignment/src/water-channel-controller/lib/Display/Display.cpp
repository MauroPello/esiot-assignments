#include <Display.hpp>

#define MAX_COLS 20
#define MAX_ROWS 4
#define LCD_ADDR 0x27

Display::Display() : lcd{LiquidCrystal_I2C(LCD_ADDR, MAX_COLS, MAX_ROWS)} {
    this->lcd.init();
    this->lcd.backlight();
}

void Display::print(String mode, int percentage) {    
    this->lcd.clear();
    this->lcd.setCursor(0, 0);
    this->lcd.print(mode);
    this->lcd.setCursor(0, 1);
    this->lcd.print(String(percentage) + "%");
}
#include <UserLCD.hpp>
#define MAX_COLS 20
#define MAX_ROWS 4
#define LCD_ADDR 0x27

UserLCD::UserLCD() : lcd{LiquidCrystal_I2C(LCD_ADDR, MAX_COLS, MAX_ROWS)} {
    this->lcd.init();
    this->lcd.backlight();
    for (int i = 0; i < sizeof(barFragment) / sizeof(barFragment[0]); i++) {
        barFragment[i] = B11111;
    }
    lcd.createChar(0, barFragment);
}

void UserLCD::resetCursor() {
    this->lcd.setCursor(0, 0);
}

void UserLCD::printColumnProgress(int colIndex) {
    for (size_t i = 0; i < MAX_ROWS; i++) {
        this->lcd.setCursor(colIndex, i);
        this->lcd.write(byte(0));
    }
}

void UserLCD::startProgressBar(int max) {
    this->resetCursor();
    this->lcd.clear();
    this->maxProgress = max;
}

void UserLCD::tickProgressBar(int progress) {
    this->resetCursor();
    int barProgress = ((float)progress * (float)MAX_COLS) / (float)maxProgress;
    this->printColumnProgress(barProgress);
}

void UserLCD::print(String msg) {
    this->lcd.clear();
    this->resetCursor();
    int i = 0;
    int cursorLine = 0;
    int cursorCol = 1;
    while (msg.length() > i) {
        this->lcd.print(msg[i]);
        this->lcd.setCursor(cursorCol, cursorLine);
        cursorCol++;
        if (cursorCol == MAX_COLS)
        {
            cursorLine++;
            cursorCol = 0;
        }
        i++;
    }
}
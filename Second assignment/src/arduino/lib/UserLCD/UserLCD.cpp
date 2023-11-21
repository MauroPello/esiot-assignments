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
    for (size_t i = 0; i < MAX_ROWS - 1; i++) {
        this->lcd.setCursor(i, colIndex);
        this->lcd.write(byte(0));
    }
}

void UserLCD::drawProgressBar(int progress, int max) {
    this->resetCursor();
    this->lcd.clear();
    int barProgress = (progress * MAX_COLS) / max;
    for (int i = 0; i < barProgress; i++) {
        this->printColumnProgress(i);
    }
}

void UserLCD::print(String msg) {
    this->resetCursor();
    int i = 0;
    int cursorLine = 0;
    int cursorCol = 0;
    while (msg[i] != '\n') {
        this->lcd.print(msg[i]);
        this->lcd.setCursor(cursorLine, cursorCol);
        cursorCol++;
        if (cursorCol == MAX_COLS - 1)
        {
            cursorLine++;
            cursorCol = 0;
        }
        i++;
    }
}
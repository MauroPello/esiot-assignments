#include <UserLCD.hpp>
#define MAX_COLS 20
#define MAX_ROWS 4
#define LCD_ADDR 0x27

UserLCD::UserLCD() : lcd{LiquidCrystal_I2C(LCD_ADDR, MAX_COLS, MAX_ROWS)} {
    this->lcd.init();
    this->lcd.backlight();
    for (unsigned int i = 0; i < sizeof(barFragment) / sizeof(barFragment[0]); i++) {
        barFragment[i] = B11111;
    }
    lcd.createChar(0, barFragment);
}

void UserLCD::resetDisplay() {
    this->lcd.setCursor(0, 0);
    this->lcd.clear();
}

void UserLCD::restartProgressBar(int max) {
    this->resetDisplay();
    for (int i = 0; i < max; i++) {
        this->tickProgressBar(i);
    }
}

void UserLCD::printColumnProgress(int colIndex) {
    for (size_t i = 0; i < MAX_ROWS; i++) {
        this->lcd.setCursor(colIndex, i);
        this->lcd.write(byte(0));
    }
}

void UserLCD::startProgressBar(int max) {
    this->resetDisplay();
    this->maxProgress = max;
}

void UserLCD::tickProgressBar(int progress) {
    this->printColumnProgress(map(progress, 0, maxProgress, 0, MAX_COLS));
}

void UserLCD::print(String msg) {
    this->resetDisplay();
    int cursorLine = 0;
    int cursorCol = 1;
    for (unsigned int i = 0; i < msg.length(); i++, cursorCol++)
    {
        this->lcd.print(msg[i]);
        this->lcd.setCursor(cursorCol, cursorLine);
        if (cursorCol == MAX_COLS)
        {
            cursorLine++;
            cursorCol = 0;
        }
    }
}
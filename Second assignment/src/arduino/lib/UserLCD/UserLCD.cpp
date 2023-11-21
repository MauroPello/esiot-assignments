#include <UserLCD.hpp>
#define MaxCols 20
#define MaxRows 4

UserLCD::UserLCD() : lcd{LiquidCrystal_I2C(0x27, MaxCols, MaxRows)} {
    this->lcd.init();
    this->lcd.backlight();
    for (int i = 0; i < sizeof(barFragment) / sizeof(barFragment[0]); i++)
    {
        barFragment[i] = B11111;
    }
    lcd.createChar(0, barFragment);
}

void UserLCD::resetCursor() {
    this->lcd.setCursor(0, 0);
}

void UserLCD::printColumnProgress(int colIndex) {
    for (size_t i = 0; i < 3; i++)
    {
        this->lcd.setCursor(i, colIndex);
        this->lcd.write(byte(0));
    }
}

void UserLCD::drawProgressBar(int progress, int max) {
    this->resetCursor();
    this->lcd.clear();
    int barProgress = (progress * 20) / max;
    for (int i = 0; i < barProgress; i++) {
        this->printColumnProgress(i);
    }
}

void UserLCD::print(String msg) {
    this->resetCursor();
    int i = 0;
    int cursorLine = 0;
    int cursorCol = 0;
    while (msg[i] != '\n')
    {
        this->lcd.print(msg[i]);
        this->lcd.setCursor(cursorLine, cursorCol);
        cursorCol++;
        if (cursorCol == 19)
        {
            cursorLine++;
            cursorCol=0;
        }
        i++;
    }
}
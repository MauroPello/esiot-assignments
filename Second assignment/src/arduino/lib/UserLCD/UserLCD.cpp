#include <UserLCD.hpp>

UserLCD::UserLCD() : lcd{LiquidCrystal_I2C(0x27, 20, 4)} {
    this->lcd.init();
    this->lcd.backlight();
}

void UserLCD::resetCursor() {

}

void UserLCD::drawProgressBar(int progress, int max) {
    this->lcd.setCursor(0, 0);
    // TODO
    this->lcd.print("Ciao");
}

void UserLCD::print(String msg) {
    this->lcd.setCursor(0, 0);
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
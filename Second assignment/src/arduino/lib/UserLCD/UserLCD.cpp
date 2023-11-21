#include <UserLCD.hpp>
#include <Arduino.h>

UserLCD::UserLCD() {
    lcd.init();
    lcd.backlight();
    // TODO: decidere se ci piace partire da 0,0
    lcd.setCursor(0, 0);
}

void UserLCD::startCounter() {
    counter = 1;
    tick();
}

void UserLCD::tick() {
    lcd.setCursor(0, 0);
    lcd.print(String(counter));
}

void UserLCD::print(char *msg) {
    lcd.setCursor(0, 0);
    int i = 0;
    int cursorLine = 0;
    int cursorCol = 0;
    while (msg[i] != '\n')
    {
        lcd.print(msg[i]);
        lcd.setCursor(cursorLine, cursorCol);
        cursorCol++;
        if (cursorCol == 19)
        {
            cursorLine++;
            cursorCol=0;
        }
        i++;
    }
}
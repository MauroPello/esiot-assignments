#include "utils.hpp"
#include "EnableInterrupt.h"
#include <avr/sleep.h>

int fadeStep = 5;
int currentBrightness = 0;
unsigned long lastPulseChange = 0;

const int buttons[N_PINS] = {4, 5, 6, 7};

void setupUtils() {
    Serial.begin(9600);
    randomSeed(analogRead(2));

    for (int i = 0; i < N_PINS; i++) {
        pinMode(greenLeds[i], OUTPUT);
        pinMode(buttons[i], INPUT_PULLUP);
    }
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
}

unsigned long int getCurrentTimeMillis() { return millis(); }

void consolePrint(String msg) { Serial.println(msg); }

void digitalOn(uint8_t pin) { digitalWrite(pin, HIGH); }

void digitalOff(uint8_t pin) { digitalWrite(pin, LOW); }

int analogPinRead(uint8_t pin) { return analogRead(pin); }

void analogOn(uint8_t pin) { analogWrite(pin, 255); }

void analogOff(uint8_t pin) { analogWrite(pin, 0); }

void wait(unsigned long ms) { delay(ms); }

long randomNumber(long min, long max) { return random(min, max); }

void sleep() { sleep_mode(); }

void fadeLed() {
    if (getCurrentTimeMillis() - lastPulseChange > PULSE_INTERVAL) {
        lastPulseChange = getCurrentTimeMillis();
        analogWrite(RED_LED_PIN, currentBrightness);
        currentBrightness += fadeStep;
        if (currentBrightness == MIN_BRIGHTNESS ||
            currentBrightness == MAX_BRIGHTNESS) {
            fadeStep *= -1;
        }
    }
}

void mutexLock() {
    noInterrupts();
}

void mutexUnlock() {
    interrupts();
}

void attachButtonInterrupt(int buttonNum, void (*function)(void)) {
    enableInterrupt(buttons[buttonNum- 1], function, RISING);
}
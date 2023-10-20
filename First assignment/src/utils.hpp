#ifndef UTILS_H
#define UTILS_H
#include <Arduino.h>

#define RED_LED_PIN 3
#define POTENTIOMETER_PIN A0
#define MICRO_TO_SECONDS(x) x * 1000000
#define MILLI_TO_SECONDS(x) x * 1000
#define MAX_ANSWERS 4 // EHM

const int greenLeds[MAX_ANSWERS] = {8, 9, 10, 11};
const int buttons[MAX_ANSWERS] = {4, 5, 6, 7};

#endif
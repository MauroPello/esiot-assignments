#ifndef UTILS_H
#define UTILS_H
#include <Arduino.h>

#define RED_LED_PIN 3
#define POTENTIOMETER_PIN A0
#define MICRO_TO_SECONDS(x) x * 1000000
#define MILLI_TO_SECONDS(x) x * 1000
#define PULSE_INTERVAL 10
#define MIN_BRIGHTNESS 0
#define MAX_BRIGHTNESS 255
#define N_PINS 4

const int greenLeds[N_PINS] = {8, 9, 10, 11};

void setupUtils();
unsigned long int getCurrentTimeMillis();
void consolePrint(String msg);
void digitalOn(uint8_t pin);
void digitalOff(uint8_t pin);
int analogPinRead(uint8_t pin);
void analogOn(uint8_t pin);
void analogOff(uint8_t pin);
void wait(unsigned long ms);
long randomNumber(long min, long max);
void sleep();
void fadeLed();
void mutexLock();
void mutexUnlock();
void attachButtonInterrupt(int buttonNum, void (*function)(void));
float mapValueFromRange(float x, float in_min, float in_max, float out_min,
                        float out_max);

#endif
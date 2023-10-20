#include "EnableInterrupt.h"
#include "game.hpp"
#include <Arduino.h>
#include <avr/sleep.h>

#define WAIT_SLEEP 10
#define PULSE_INTERVAL 10
#define MIN_BRIGHTNESS 0
#define MAX_BRIGHTNESS 255

int fadeStep = 5;
int currentBrightness = 0;

void button1Pressed() {
    if (gameState == INIT) {
        gameState = IN_GAME;
    } else if (gameState == WAITING_ANS) {
        checkAnswer(1);
    }
}

void button2Pressed() {
    if (gameState == WAITING_ANS) {
        checkAnswer(2);
    }
}

void button3Pressed() {
    if (gameState == WAITING_ANS) {
        checkAnswer(3);
    }
}

void button4Pressed() {
    if (gameState == WAITING_ANS) {
        checkAnswer(4);
    }
}

void setup() {
    Serial.begin(9600);
    randomSeed(analogRead(2));

    for (int i = 0; i < MAX_ANSWERS; i++) {
        pinMode(greenLeds[i], OUTPUT);
        pinMode(buttons[i], INPUT_PULLUP);
    }
    enableInterrupt(buttons[0], button1Pressed, RISING);
    enableInterrupt(buttons[1], button2Pressed, RISING);
    enableInterrupt(buttons[2], button3Pressed, RISING);
    enableInterrupt(buttons[3], button4Pressed, RISING);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    gameState = SETUP;
}

void loop() {
    switch (gameState) {
    case SETUP:
        initGame();
        break;
    case INIT:
        if (millis() - lastPulseChange > PULSE_INTERVAL) {
            lastPulseChange = millis();
            analogWrite(RED_LED_PIN, currentBrightness);
            currentBrightness += fadeStep;
            if (currentBrightness == MIN_BRIGHTNESS || currentBrightness == MAX_BRIGHTNESS) {
                fadeStep *= -1;
            }
        }
        if (millis() - lastInit > MILLI_TO_SECONDS(WAIT_SLEEP)) {
            // Se scade il timer mentre viene premuto B1 è necessario non sovrascrivere il cambio di stato
            noInterrupts();
            if (gameState == INIT) {
                gameState = SLEEPING;
            }
            interrupts();
        }
        break;
    case IN_GAME:
        startGame();
        break;
    case IN_ROUND:
        startRound();
        break;
    case WAITING_ANS:
        checkGameWon();
        break;
    case SLEEPING:
        analogWrite(RED_LED_PIN, 0);
        sleep_mode();
        gameState = SETUP;
    case GAME_OVER:
        gameOver();
        break;
    default:
        break;
    }
}
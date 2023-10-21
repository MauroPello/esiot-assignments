#include "game.hpp"
#include "utils.hpp"

enum GameState gameState;
double T1;
double T2;
double T3;
double F;

int score;
int correctAnswersGiven;
int correctAnswers[MAX_ANSWERS];
int buttonsPressed[MAX_ANSWERS];

unsigned long lastInit = 0;
unsigned long lastAnswerWait = 0;

void checkAnswer(int buttonNum) {
    if (buttonsPressed[buttonNum - 1]) {
        return;
    }

    buttonsPressed[buttonNum - 1] = true;
    if (correctAnswers[correctAnswersGiven] != buttonNum) {
        gameState = GAME_OVER;
    } else {
        correctAnswersGiven++;
    }
}

void initGame() {
    consolePrint(START_GAME_MSG);
    for (int i = 0; i < MAX_ANSWERS; i++) {
        digitalOff(greenLeds[i]);
    }
    gameState = INIT;

    lastInit = getCurrentTimeMillis();
}

void startRound() {
    for (int i = 0; i < MAX_ANSWERS; i++) {
        digitalOff(greenLeds[i]);
    }
    analogOff(RED_LED_PIN);
    consolePrint(START_ROUND_MSG);

    wait(MILLI_TO_SECONDS(T1));
    for (int i = 0; i < MAX_ANSWERS; i++) {
        digitalOn(greenLeds[i]);
        correctAnswers[i] = i + 1;
    }

    for (int i = MAX_ANSWERS - 1; i > 0; i--) {
        int j = randomNumber(0, MAX_ANSWERS);
        int tmp = correctAnswers[i];
        correctAnswers[i] = correctAnswers[j];
        correctAnswers[j] = tmp;
    }
    for (int i = MAX_ANSWERS - 1; i >= 0; i--) {
        wait(MILLI_TO_SECONDS(T2 / MAX_ANSWERS));
        digitalOff(greenLeds[correctAnswers[i] - 1]);
    }

    correctAnswersGiven = 0;
    for (int i = 0; i < MAX_ANSWERS; i++) {
        buttonsPressed[i] = false;
    }
    gameState = WAITING_ANS;
    lastAnswerWait = getCurrentTimeMillis();
}

void startGame() {
    int L = mapValueFromRange(analogPinRead(POTENTIOMETER_PIN), 0, 1023, MIN_L, MAX_L);
    F = mapValueFromRange(L, MIN_L, MAX_L, MIN_F, MAX_F);
    score = 0;

    T1 = randomNumber(MIN_T1, MAX_T1 + 1);
    T2 = DEFAULT_T2;
    T3 = DEFAULT_T3;

    gameState = IN_ROUND;
}

void checkGameWon() {
    if (getCurrentTimeMillis() - lastAnswerWait > MILLI_TO_SECONDS(T3)) {
        gameState = GAME_OVER;
    }
    if (correctAnswersGiven == MAX_ANSWERS) {
        score++;
        T2 *= F;
        T3 *= F;
        consolePrint(ROUND_WON_MSG(score));
        gameState = IN_ROUND;
    }
}

void gameOver() {
    analogOn(RED_LED_PIN);
    wait(MILLI_TO_SECONDS(WAIT_GAME_OVER));
    analogOff(RED_LED_PIN);
    consolePrint(GAME_OVER_MSG(score));
    wait(MILLI_TO_SECONDS(WAIT_RESTART));
    gameState = SETUP;
}

void goToSleep() {
    analogOff(RED_LED_PIN);
    sleep();
}

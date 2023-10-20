#include "game.hpp"

enum GameState gameState;
double T1;
double T2;
double T3;
double F;

int score;
int correctAnswersGiven;
int correctAnswers[MAX_ANSWERS];
int buttonsPressed[MAX_ANSWERS];

unsigned long lastPulseChange = 0;
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
    Serial.println(START_GAME_MSG);
    for (int i = 0; i < MAX_ANSWERS; i++) {
        digitalWrite(greenLeds[i], LOW);
    }
    gameState = INIT;

    lastInit = millis();
}

void startRound() {
    for (int i = 0; i < MAX_ANSWERS; i++) {
        digitalWrite(greenLeds[i], LOW);
    }
    analogWrite(RED_LED_PIN, 0);
    Serial.println(START_ROUND_MSG);

    delay(MILLI_TO_SECONDS(T1));
    for (int i = 0; i < MAX_ANSWERS; i++) {
        digitalWrite(greenLeds[i], HIGH);
        correctAnswers[i] = i + 1;
    }

    for (int i = MAX_ANSWERS - 1; i > 0; i--) {
        int j = random(MAX_ANSWERS);
        int tmp = correctAnswers[i];
        correctAnswers[i] = correctAnswers[j];
        correctAnswers[j] = tmp;
    }
    for (int i = MAX_ANSWERS - 1; i >= 0; i--) {
        delay(MILLI_TO_SECONDS(T2 / MAX_ANSWERS));
        digitalWrite(greenLeds[correctAnswers[i] - 1], LOW);
    }

    correctAnswersGiven = 0;
    for (int i = 0; i < MAX_ANSWERS; i++) {
        buttonsPressed[i] = false;
    }
    gameState = WAITING_ANS;
    lastAnswerWait = millis();
}

void startGame() {
    int L = map(analogRead(POTENTIOMETER_PIN), 0, 1023, MIN_L, MAX_L);
    F = MIN_F + (MAX_F - MIN_F) / L; //MAX_F non sarà mai raggiunto
    score = 0;

    T1 = random(MIN_T1, MAX_T1 + 1);
    T2 = DEFAULT_T2;
    T3 = DEFAULT_T3;

    gameState = IN_ROUND;
}

void checkGameWon() {
    if (millis() - lastAnswerWait > MILLI_TO_SECONDS(T3)) {
        gameState = GAME_OVER;
    }
    if (correctAnswersGiven == MAX_ANSWERS) {
        score++;
        T2 *= F;
        T3 *= F;
        Serial.println(ROUND_WON_MSG(score));
        gameState = IN_ROUND;
    }
}

void gameOver() {
    analogWrite(RED_LED_PIN, 255);
    delay(MILLI_TO_SECONDS(WAIT_GAME_OVER));
    Serial.println(GAME_OVER_MSG(score));
    delay(MILLI_TO_SECONDS(WAIT_RESTART));
    gameState = SETUP;
}

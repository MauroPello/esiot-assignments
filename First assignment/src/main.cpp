// Babini, Marrani, Pellonara, Speziali

#include "game.hpp"
#include "utils.hpp"

#define WAIT_SLEEP 10

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
    attachButtonInterrupt(1, button1Pressed);
    attachButtonInterrupt(2, button2Pressed);
    attachButtonInterrupt(3, button3Pressed);
    attachButtonInterrupt(4, button4Pressed);
    setupUtils();
    gameState = SETUP;
}

void loop() {
    switch (gameState) {
    case SETUP:
        initGame();
        break;
    case INIT:
        fadeLed();
        if (getCurrentTimeMillis() - lastInit > MILLI_TO_SECONDS(WAIT_SLEEP)) {
            // Se scade il timer mentre viene premuto B1 è necessario non
            // sovrascrivere il cambio di stato
            mutexLock();
            if (gameState == INIT) {
                gameState = SLEEPING;
            }
            mutexUnlock();
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
        goToSleep();
        gameState = SETUP;
        break;
    case GAME_OVER:
        gameOver();
        break;
    default:
        break;
    }
}
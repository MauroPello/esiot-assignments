#ifndef GAME_H
#define GAME_H

#define START_GAME_MSG                                                         \
    "Welcome to the Restore the Light Game. Press Key B1 to Start"
#define START_ROUND_MSG "Go!"
#define GAME_OVER_MSG(x) String("Game Over. Final Score: ") + x
#define ROUND_WON_MSG(x) String("New point! Score: ") + x
#define MIN_T1 2
#define MAX_T1 5
#define MIN_F 0.3
#define MAX_F 0.9
#define MIN_L 1
#define MAX_L 4
#define DEFAULT_T2 4
#define DEFAULT_T3 5
#define WAIT_GAME_OVER 1
#define WAIT_RESTART 10
#define MAX_ANSWERS N_PINS

enum GameState {
    SETUP,
    INIT,
    IN_GAME,
    IN_ROUND,
    WAITING_ANS,
    SLEEPING,
    GAME_OVER
};

extern enum GameState gameState;
extern unsigned long lastInit;

void checkAnswer(int buttonNum);
void initGame();
void startRound();
void startGame();
void checkGameWon();
void gameOver();
void goToSleep();

#endif
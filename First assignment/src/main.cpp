#include <Arduino.h>
#include <avr/sleep.h>
#include "EnableInterrupt.h"

#define RED_LED_PIN 3
#define POT_PIN A0
#define N_GREEN_LEDS 4
#define START_GAME_MSG "Welcome to the Restore the Light Game. Press Key B1 to Start"
#define START_ROUND_MSG "Go!"
#define GAME_OVER_MSG(x) String("Game Over. Final Score: ") + x
#define ROUND_WON_MSG(x) String("New point! Score: ") + x
#define MICRO_TO_SECONDS(x) x * 1000000
#define MILLI_TO_SECONDS(x) x * 1000
#define WAIT_TIME 10
#define MIN_T1 2
#define MAX_T1 5
#define MIN_F 0.3
#define MAX_F 0.9

const int greenLeds[N_GREEN_LEDS] = {8, 9, 10, 11};
const int buttons[N_GREEN_LEDS] = {4, 5, 6, 7};

enum GameState
{
    SETUPPED,
    INITIALIZED,
    GAME_STARTED,
    START_NEW_ROUND,
    WAITING_ANS,
    TO_SLEEP,
    SLEEPING,
    GAME_OVER
} gameState;

double T1;
double T2;
double T3;
double F;

int score;
int answersGiven;
int correctAnswers[N_GREEN_LEDS];
int buttonsPressed[N_GREEN_LEDS];

int fadeAmount = 5;
int currentIntensity = 0;
unsigned long lastPulseChange = 0;

unsigned long lastPulseStart = 0;
unsigned long lastAnswerWait = 0;

void checkAnswer(int buttonNum)
{
    if (buttonsPressed[buttonNum - 1])
    {
        return;
    }

    buttonsPressed[buttonNum - 1] = true;
    if (correctAnswers[answersGiven] != buttonNum)
    {
        gameState = GAME_OVER;
        return;
    }
    answersGiven++;
}

void button1Pressed()
{
    if (gameState == INITIALIZED)
    {
        gameState = GAME_STARTED;
        return;
    }
    else if (gameState == WAITING_ANS)
    {
        checkAnswer(1);
    }
}

void button2Pressed()
{
    if (gameState == WAITING_ANS)
    {
        checkAnswer(2);
    }
}

void button3Pressed()
{
    if (gameState == WAITING_ANS)
    {
        checkAnswer(3);
    }
}

void button4Pressed()
{
    if (gameState == WAITING_ANS)
    {
        checkAnswer(4);
    }
}

void initGame()
{
    Serial.println(START_GAME_MSG);
    for (int i = 0; i < N_GREEN_LEDS; i++)
    {
        digitalWrite(greenLeds[i], LOW);
    }
    gameState = INITIALIZED;

    lastPulseStart = millis();
}

void startRound()
{
    answersGiven = 0;
    for (int i = 0; i < N_GREEN_LEDS; i++)
    {
        digitalWrite(greenLeds[i], LOW);
    }
    digitalWrite(RED_LED_PIN, LOW);
    Serial.println(START_ROUND_MSG);

    delay(MILLI_TO_SECONDS(T1));
    for (int i = 0; i < N_GREEN_LEDS; i++)
    {
        digitalWrite(greenLeds[i], HIGH);
        correctAnswers[i] = i + 1;
    }

    for (int i = N_GREEN_LEDS - 1; i > 0; i--)
    {
        int j = random(N_GREEN_LEDS);
        int tmp = correctAnswers[i];
        correctAnswers[i] = correctAnswers[j];
        correctAnswers[j] = tmp;
    }
    for (int i = N_GREEN_LEDS - 1; i >= 0; i--)
    {
        delay(MILLI_TO_SECONDS(T2 / N_GREEN_LEDS));
        digitalWrite(greenLeds[correctAnswers[i] - 1], LOW);
    }

    for (int i = 0; i < N_GREEN_LEDS; i++)
    {
        buttonsPressed[i] = false;
    }
    gameState = WAITING_ANS;
    lastAnswerWait = millis();
}

void startGame()
{
    // blu = 0, graffetta = 1023
    int L = map(analogRead(POT_PIN), 0, 1023, 1, 4);
    F = MIN_F + (MAX_F - MIN_F) / L;
    score = 0;

    T1 = random(MIN_T1, MAX_T1 + 1);
    T2 = 4;
    T3 = 5;

    gameState = START_NEW_ROUND;
}

void setup()
{
    Serial.begin(9600);
    randomSeed(analogRead(2));

    for (int i = 0; i < N_GREEN_LEDS; i++)
    {
        pinMode(greenLeds[i], OUTPUT);
        pinMode(buttons[i], INPUT_PULLUP);
    }
    enableInterrupt(buttons[0], button1Pressed, RISING);
    enableInterrupt(buttons[1], button2Pressed, RISING);
    enableInterrupt(buttons[2], button3Pressed, RISING);
    enableInterrupt(buttons[3], button4Pressed, RISING);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    gameState = SETUPPED;
}

void loop()
{
    switch (gameState)
    {
    case SETUPPED:
        initGame();
        break;
    case INITIALIZED:
        if (millis() - lastPulseChange > 10)
        {
            lastPulseChange = millis();
            analogWrite(RED_LED_PIN, currentIntensity);
            currentIntensity = currentIntensity + fadeAmount;
            if (currentIntensity == 0 || currentIntensity == 255)
            {
                fadeAmount = -fadeAmount;
            }
        }
        // se scadono i 10s mentre si stava premendo b1
        if (millis() - lastPulseStart > MILLI_TO_SECONDS(WAIT_TIME))
        {
            noInterrupts();
            // chiedere a ricci, l'idea è che ci possa essere un interrupt che cambia lo stato di gameState quando ormai l'esecuzione
            // del superloop è dentro questo case. In questo caso, l'unica cosa che può succedere è che l'utente prema
            // il tasto B1 proprio nel momento del controllo dell'IF soprastante, portando ad avere un'incosistenza dello stato
            // di gameState
            if (gameState == INITIALIZED)
            {
                gameState = TO_SLEEP;
            }
            interrupts();
        }
        break;
    case GAME_STARTED:
        startGame();
        break;
    case START_NEW_ROUND:
        startRound();
        break;
    case WAITING_ANS:
        if (millis() - lastAnswerWait > MILLI_TO_SECONDS(T3))
        {
            gameState = GAME_OVER;
        }
        if (answersGiven == 4)
        {
            score++;
            T2 *= F;
            T3 *= F;
            Serial.println(ROUND_WON_MSG(score));
            gameState = START_NEW_ROUND;
        }
        break;
    case TO_SLEEP:
        analogWrite(RED_LED_PIN, 0);
        gameState = SLEEPING;
        sleep_mode();
        gameState = SETUPPED;
    case GAME_OVER:
        analogWrite(RED_LED_PIN, 255);
        delay(MILLI_TO_SECONDS(1));
        Serial.println(GAME_OVER_MSG(score));
        delay(MILLI_TO_SECONDS(10));
        gameState = SETUPPED;
        break;
    default:
        break;
    }
}
#pragma once
#include "bot.h"

class AIControl
{
private:
    Ball* ball;
    Character * character;
    float x_dubao;
    int counter = 0;
public:
    AIControl(Character * character, Ball* ball);
    ~AIControl();
    void SetBall(Ball* newball){this->ball = newball;}
    PlayerAction Control();
};


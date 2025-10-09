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
    void UpdateAI(){
        // cout<<"New Motition"<<endl;
        this->x_dubao = this->ball->Get_x_dubao(this->character->GetY() + 65 );
    }
};


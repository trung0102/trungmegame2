#include "AI.h"
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 750;


AIControl::AIControl(Character * character, Ball* ball){
    this->character = character;
    this->ball = ball;
    if (ball && character)
        this->x_dubao = ball->Get_x_dubao(character->GetY() + 65);
    else
        this->x_dubao = 0.0f;
}

AIControl::~AIControl(){
    this->character = nullptr;
    this->ball = nullptr;
}

PlayerAction AIControl::Control(){
    if (!ball){
        cout<<"Not Ball"<<endl;
    }
    float x = this->x_dubao;
    float X_char = this->character->GetX();
    // cout<<"X du bao "<<x<<" - "<<this->ball->GetX()<<"      X char "<<X_char<<endl;
    if (x < 0 || x > SCREEN_WIDTH)
        return PlayerAction::Idle;
    if(this->x_dubao > 500 && !this->character->isLeft()){  
        if(fabs(this->ball->GetX() - this->x_dubao) <= 15 ||(this->ball->GetY()+20 > 565)){
            cout<<"PASS"<<endl;
            if(this->counter++ <7) return PlayerAction::Pass;
        }
        else if(X_char-1 > x){
            // cout<<"Forward  "<<X_char<<endl;
            return PlayerAction::MoveForward;
        }
        else if(X_char-1 + 20 < x){
            // cout<<"Backward  "<<X_char<<endl;
            return PlayerAction::MoveBackward;
        }
    }
    else if(this->x_dubao < 500 && this->character->isLeft()){  
        if(fabs(this->ball->GetX() - this->x_dubao) <= 15 ||(this->ball->GetY()+50 > 565)){
            cout<<"PASS"<<endl;
            if(this->counter++ <7) return PlayerAction::Pass;
        }
        else if(X_char + 30 + 20 < x){
            // cout<<"Forward  "<<X_char<<endl;
            return PlayerAction::MoveForward;
        }
        else if(X_char + 30 > x){
            // cout<<"Backward  "<<X_char<<endl;
            return PlayerAction::MoveBackward;
        }
    }
    // cout<<"Idle"<<endl;
    this->counter = 0;
    return PlayerAction::Idle;
}
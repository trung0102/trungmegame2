#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include <unordered_map>
#include <tuple>
#include <iostream>
#include <cmath>
#include <queue>
using namespace std;

extern SDL_Renderer* gRenderer;
SDL_Texture* loadTexture( std::string path, SDL_Renderer* gRenderer);

enum class PlayerAction {
    Idle,                           // dung yen
    MoveForward,                    // tien
    MoveBackward,                   // lui
    Spike,                          // dap bong
    Block,                          // bật nhảy chắn bóng đối phương.
    Dive,                           // lao người cứu bóng
    Pass                            // chuyền bóng thấp
};

struct ActionData{
    int maxframe;
    string surface;
}; 
// maxframe, url_asset

extern int LEFT, RIGHT;

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
    Vec2() = default;
    Vec2(float _x, float _y): x(_x), y(_y) {}
    Vec2 operator+(const Vec2& o) const { return {x+o.x, y+o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x-o.x, y-o.y}; }
    Vec2 operator*(float s) const { return {x*s, y*s}; }
    Vec2& operator+=(const Vec2& o){ x+=o.x; y+=o.y; return *this;}
    Vec2& operator*=(float s){ x*=s; y*=s; return *this;}
    Vec2 normalize() const {
        float mag = std::sqrt(x * x + y * y);
        return mag > 0 ? Vec2(x / mag, y / mag) : *this;
    }
    bool operator==(const Vec2& other) const {
        return x == other.x && y == other.y;
    }
    float dodaibinh() const {return x*x + y*y;}
    float dot(const Vec2& v) const { return x * v.x + y * v.y ; } // tích vô hướng
};

struct KeyMap {
    SDL_Scancode up;
    SDL_Scancode down;
    SDL_Scancode left;
    SDL_Scancode right;
    bool operator==(const KeyMap& other) const {
        return up == other.up &&
               down == other.down &&
               left == other.left &&
               right == other.right;
    }
};
struct CharCollisionBall{
    bool is_collision;
    float v0;
    float alpha;
    PlayerAction action;

};

// Map từ enum -> dữ liệu
extern unordered_map<PlayerAction, ActionData> actionData;

class AIControl;
class Ball;
class MotionEquation;

class Character
{
private:
    /* data */
    PlayerAction status = PlayerAction::Idle;
    tuple<int, int> position;
    tuple<int, int> pos_0;
    tuple<int, int> patrol_range;
    int current_frame = 0;
    int max_frame;
    int speed = 12;
    SDL_Texture* surface;
    SDL_Texture* muiten;
    SDL_FRect srcRect;
    SDL_FRect dstRect;
    int y0 = 0;
    string name;
    SDL_Renderer* gRenderer;
    SDL_FlipMode flipType;
    KeyMap keymap;
    bool is_control;
    AIControl* AIcontrol;

public:
    Character(SDL_Renderer* gRenderer, tuple<int, int> position, tuple<int, int> patrol_range, string name = "Char1", bool is_control=true);
    ~Character();
    void render();
    void update_position();
    void getKeyboardEvent(SDL_KeyboardEvent keyEvent);
    CharCollisionBall checkCollision(const SDL_FRect& b);
    void changeControl(){this->is_control = !this->is_control;}
    float GetY(){return float(get<1>(this->position));}
    float GetX(){return float(get<0>(this->position));}
    bool isLeft();
    void SetAIControl(Ball* ball);
    void setPosition(){
        this->position = this->pos_0;
        this->status = PlayerAction::Idle;
    }
    string GetName(){return this->name;}
    PlayerAction GetStatus(){return this->status;}
    void UpdateAI();
};  

// inline float deg2rad(float deg){ return deg * 3.14159265358979323846f / 180.0f; }
class MotionEquation
{
private:
    float alpha;    // vị trí ban đầu
    float v0,x0,y0;    // vận tốc ban đầu, tọa độ ban đầu
    float a,b,c;     // ax2 + bx + c
    int idx = 1;
    Vec2 direction_vec;
public:
    MotionEquation(float alpha, float v0,float x0 =0,float y0=0, Vec2 a=Vec2(0,0));
    ~MotionEquation();
    tuple<int,int> position(float dt);
    string print();
    Vec2 direction_vector(float x0);
    float getV0() {return this->v0;}
    float getAlpha() {return this->alpha;}
    float SolveEquation(float y = 555);
};
class Ball
{
private:
    /* data */
    int current_frame;
    int max_frame;
    tuple<int, int> position;
    SDL_Texture* surface;
    SDL_FRect srcRect;
    SDL_FRect dstRect;
    MotionEquation * motition = nullptr;
    int idex = 0;
    int isdead = 0;
    SDL_Renderer* gRenderer;
    queue <tuple<int, int>> queue_pos;
    SDL_Texture *duanh;
    SDL_Texture *dubao;
    bool can_touch = true;
    float y_dubao;
    float x_dubao;
    string char_create_motition = "";
    bool create_new_motition = false;
    
public:
    Ball(SDL_Renderer* gRenderer, tuple<int, int> position,string a="LEFT");
    ~Ball();
    bool update_position();
    void render();
    void collide(string str);
    bool Isdead(){ return this->isdead == 5;}
    void checkCollision(Character* character);
    float Get_x_dubao(float y){
        return this->motition->SolveEquation(y);
    }
    float GetX(){return float(get<0>(this->position));}
    float GetY(){return float(get<1>(this->position));}
    bool is_new_motition(){return this->create_new_motition;}
    void renderBallEffects();
    void print(){
        cout<< "Ball:  x: "<<get<0>(this->position)<<"   y: "<<get<1>(this->position)<<endl;
    }
};

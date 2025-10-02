#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include <unordered_map>
#include <tuple>
#include <iostream>
#include <cmath>
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

struct Vec2;

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
};

// Map từ enum -> dữ liệu
extern unordered_map<PlayerAction, ActionData> actionData;

class Character
{
private:
    /* data */
    PlayerAction status = PlayerAction::Idle;
    tuple<int, int> position;
    tuple<int, int> patrol_range;
    int current_frame = 0;
    int max_frame;
    int speed = 12;
    SDL_Texture* surface;
    SDL_FRect srcRect;
    SDL_FRect dstRect;
    int y0 = 0;
    string name;
    SDL_Renderer* gRenderer;
    SDL_FlipMode flipType;
    KeyMap keymap;

public:
    Character(SDL_Renderer* gRenderer, tuple<int, int> position, tuple<int, int> patrol_range, string name = "Char1");
    ~Character();
    void render();
    void update_position();
    void getKeyboardEvent(SDL_KeyboardEvent keyEvent);
    CharCollisionBall checkCollision(const SDL_FRect& b);
};

// inline float deg2rad(float deg){ return deg * 3.14159265358979323846f / 180.0f; }
class MotionEquation
{
private:
    float alpha;    // vị trí ban đầu
    float v0,x0,y0;    // vận tốc ban đầu, tọa độ ban đầu
    float a,b,c;     // ax2 + bx + c
    int idx = 1;
public:
    MotionEquation(float alpha, float v0,float x0 =0,float y0=0);
    ~MotionEquation();
    tuple<int,int> position(float dt);
    string print();
    Vec2 direction_vector(float x0);
    float getV0() {return this->v0;}
    float getAlpha() {return this->alpha;}
};
class Ball
{
private:
    /* data */
    int current_frame;
    int max_frame;
    int speed = 0.5;
    tuple<int, int> position;
    SDL_Texture* surface;
    SDL_FRect srcRect;
    SDL_FRect dstRect;
    MotionEquation * motition = nullptr;
    int idex = 0;
    int isdead = 0;
    SDL_Renderer* gRenderer;
    
public:
    Ball(SDL_Renderer* gRenderer, tuple<int, int> position);
    ~Ball();
    void update_position();
    void render();
    void collide(string str);
    bool Isdead(){ return this->isdead == 5;}
    void checkCollision(Character* character);
};

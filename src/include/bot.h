#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include <unordered_map>
#include <tuple>
#include <iostream>
using namespace std;

SDL_Surface* loadSurface( std::string path);

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
    SDL_Surface* surface;
}; 
// maxframe, url_asset

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
    int speed = 7;
    SDL_Surface* surface;
    SDL_Rect srcRect;
    SDL_Rect dstRect;

public:
    Character(tuple<int, int> position, tuple<int, int> patrol_range);
    ~Character();
    void render(SDL_Surface* screenSurface);
    void update_position();
    void getKeyboardEvent(SDL_KeyboardEvent keyEvent);
};

class Ball
{
private:
    /* data */
    int current_frame;
    int max_frame;
    int speed = 0.5;
    tuple<int, int> position;
    SDL_Surface* surface;
    SDL_Rect srcRect = {0, 0, 32, 48};
    SDL_Rect dstRect = {500, 500, 32*2, 48*2};
    
public:
    Ball(tuple<int, int> position, int max_frame);
    ~Ball();
    void update_position();
    void render(SDL_Surface* screenSurface);
};

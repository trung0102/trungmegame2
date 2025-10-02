#ifndef BALL_H
#define BALL_H

#include <SDL2/SDL.h>
#include "Player.h"

class Ball {
private:
    float x, y;
    float vx, vy;
    float radius;
    bool active;
    int lastHitBy;
    int touchCount;
    int lastTouchedTeam;

    static constexpr float GRAVITY = 0.2f;
    static constexpr float BALL_BOUNCE = 0.8f;

    // Textures
    SDL_Texture* textureFull;
    SDL_Texture* textureBounce;
    SDL_Texture* textureBounceHard;
    SDL_Texture* textureRoll;
    SDL_Texture* textureShot;
    SDL_Texture* currentTexture;

    SDL_Renderer* renderer;

    // Animation frame data
    int currentFrame;
    int maxFrames;
    int frameWidth;
    int frameHeight;
    Uint32 lastFrameTime;

public:
    Ball(float startX, float startY, float r = 15);
    ~Ball();

    void loadTextures(SDL_Renderer* ren);
    void update(float groundY);
    void reset(int scoredBy = 0, int screenWidth = 800);
    bool checkCollision(Player& player, bool isPlayer1, int netX);
    void checkCrossedNet(int netX, int& previousSide);

    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getRadius() const { return radius; }
    float getVelocityY() const { return vy; }
    bool isActive() const { return active; }
    int getTouchCount() const { return touchCount; }
    int getLastTouchedTeam() const { return lastTouchedTeam; }
    bool checkTouchViolation() const { return touchCount > 3; }
    SDL_Texture* getTexture() const { return currentTexture; }
    SDL_Rect getSrcRect() const;  // Get current frame rect

    SDL_FRect getBounds() const;
};

#endif // BALL_H

#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>

enum class PlayerState {
    IDLE,
    RUN,
    JUMP,
    SMASH,
    BLOCK,
    RECEPTION
};

class Player {
private:
    float x, y;
    float vx, vy;
    float width, height;
    bool onGround;
    int score;
    float initialX, initialY;

    static constexpr float GRAVITY = 0.2f;
    static constexpr float PLAYER_JUMP = -12.0f;

    // Textures
    SDL_Texture* textureIdle;
    SDL_Texture* textureRun;
    SDL_Texture* textureSmash;
    SDL_Texture* textureBlock;
    SDL_Texture* textureReception;
    SDL_Texture* currentTexture;

    PlayerState currentState;
    SDL_Renderer* renderer;
    bool facingRight;

    Uint32 smashTime;
    Uint32 blockTime;
    Uint32 receptionTime;

    float boundaryMinX;
    float boundaryMaxX;

    // Animation frame data
    int currentFrame;
    int maxFrames;
    int frameWidth;
    int frameHeight;
    Uint32 lastFrameTime;

public:
    static constexpr float PLAYER_SPEED = 5.0f;
    Player(float startX, float startY, float w = 30, float h = 60);
    virtual ~Player();

    void loadTextures(SDL_Renderer* ren, bool isPlayer2 = false);
    void update(float groundY);
    void jump();
    void resetPosition();
    void setSmashing() { smashTime = SDL_GetTicks(); currentState = PlayerState::SMASH; }
    void setBlocking() { blockTime = SDL_GetTicks(); currentState = PlayerState::BLOCK; }
    void setReceiving() { receptionTime = SDL_GetTicks(); currentState = PlayerState::RECEPTION; }
    void setBoundaries(float minX, float maxX);  // Set court boundaries

    // Setters
    void setVelocityX(float velocity) { vx = velocity; }
    void setVelocityY(float velocity) { vy = velocity; }
    void addScore() { score++; }
    void resetScore() { score = 0; }

    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    int getScore() const { return score; }
    bool isOnGround() const { return onGround; }
    SDL_Texture* getTexture() const { return currentTexture; }
    bool isFacingRight() const { return facingRight; }
    SDL_Rect getSrcRect() const;  // Get current frame rect

    SDL_FRect getBounds() const;
};

#endif // PLAYER_H

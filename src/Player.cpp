#include "../include/Player.h"
#include <SDL2/SDL_image.h>
#include <algorithm>
#include <iostream>

Player::Player(float startX, float startY, float w, float h)
    : x(startX), y(startY), vx(0), vy(0),
      width(w), height(h), onGround(false), score(0),
      initialX(startX), initialY(startY),
      textureIdle(nullptr), textureRun(nullptr),
      textureSmash(nullptr), textureBlock(nullptr),
      textureReception(nullptr), currentTexture(nullptr),
      currentState(PlayerState::IDLE), renderer(nullptr),
      facingRight(true), smashTime(0), blockTime(0), receptionTime(0),
      boundaryMinX(0), boundaryMaxX(800),
      currentFrame(0), maxFrames(12), frameWidth(32), frameHeight(48), lastFrameTime(0) {
}

Player::~Player() {
    if (textureIdle) SDL_DestroyTexture(textureIdle);
    if (textureRun) SDL_DestroyTexture(textureRun);
    if (textureSmash) SDL_DestroyTexture(textureSmash);
    if (textureBlock) SDL_DestroyTexture(textureBlock);
    if (textureReception) SDL_DestroyTexture(textureReception);
}

void Player::loadTextures(SDL_Renderer* ren, bool isPlayer2) {
    renderer = ren;
    facingRight = !isPlayer2;  // Player 2 faces left by default

    SDL_Surface* surface = IMG_Load("assets/playerIdle.png");
    if (surface) {
        textureIdle = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    surface = IMG_Load("assets/playerRun.png");
    if (surface) {
        textureRun = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    surface = IMG_Load("assets/playerSmash.png");
    if (surface) {
        textureSmash = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    surface = IMG_Load("assets/playerBlock.png");
    if (surface) {
        textureBlock = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    surface = IMG_Load("assets/playerReception.png");
    if (surface) {
        textureReception = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    currentTexture = textureIdle;
}

void Player::update(float groundY) {
    x += vx;
    y += vy;
    vy += GRAVITY;

    // Check collision with ground
    if (y + height >= groundY) {
        y = groundY - height;
        vy = 0;
        onGround = true;
    } else {
        onGround = false;
    }

    // Limit within court boundaries
    if (x < boundaryMinX) x = boundaryMinX;
    if (x + width > boundaryMaxX) x = boundaryMaxX - width;

    // Update facing direction based on velocity
    if (vx > 0.1f) facingRight = true;
    else if (vx < -0.1f) facingRight = false;

    // Update animation state based on movement and timed actions
    Uint32 currentTime = SDL_GetTicks();

    // Store previous state to detect changes
    PlayerState previousState = currentState;

    // Check if timed actions have expired (300ms duration)
    if (currentState == PlayerState::SMASH && currentTime - smashTime > 300) {
        currentState = PlayerState::IDLE;
    }
    if (currentState == PlayerState::BLOCK && currentTime - blockTime > 300) {
        currentState = PlayerState::IDLE;
    }
    if (currentState == PlayerState::RECEPTION && currentTime - receptionTime > 300) {
        currentState = PlayerState::IDLE;
    }

    // Update state based on movement (if not in timed action)
    if (currentState == PlayerState::IDLE || currentState == PlayerState::RUN || currentState == PlayerState::JUMP) {
        if (!onGround) {
            currentState = PlayerState::JUMP;
        } else if (vx != 0) {
            currentState = PlayerState::RUN;
        } else {
            currentState = PlayerState::IDLE;
        }
    }

    // Reset frame if state changed
    if (previousState != currentState) {
        currentFrame = 0;
    }

    // Update texture and max frames based on state
    if (currentState == PlayerState::SMASH) {
        currentTexture = textureSmash ? textureSmash : textureIdle;
        maxFrames = 13;
    } else if (currentState == PlayerState::BLOCK) {
        currentTexture = textureBlock ? textureBlock : textureIdle;
        maxFrames = 13;
    } else if (currentState == PlayerState::RECEPTION) {
        currentTexture = textureReception ? textureReception : textureIdle;
        maxFrames = 11;
    } else if (currentState == PlayerState::JUMP) {
        currentTexture = textureSmash ? textureSmash : textureIdle;
        maxFrames = 13;
    } else if (currentState == PlayerState::RUN) {
        currentTexture = textureRun ? textureRun : textureIdle;
        maxFrames = 12;
    } else {
        currentTexture = textureIdle ? textureIdle : currentTexture;
        maxFrames = 12;
    }

    // Update animation frame (60ms per frame for smooth animation)
    if (currentTime - lastFrameTime >= 60) {
        currentFrame = (currentFrame + 1) % maxFrames;
        lastFrameTime = currentTime;
    }
}

void Player::jump() {
    if (onGround) {
        vy = PLAYER_JUMP;
        onGround = false;
    }
}

void Player::resetPosition() {
    x = initialX;
    y = initialY;
    vx = 0;
    vy = 0;
}

void Player::setBoundaries(float minX, float maxX) {
    boundaryMinX = minX;
    boundaryMaxX = maxX;
}

SDL_Rect Player::getSrcRect() const {
    // Return source rectangle for current frame in spritesheet
    return {currentFrame * frameWidth, 0, frameWidth, frameHeight};
}

SDL_FRect Player::getBounds() const {
    return {x, y, width, height};
}

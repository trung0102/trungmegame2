#include "../include/Ball.h"
#include <cmath>
#include <algorithm>
#include <SDL2/SDL_image.h>
#include <iostream>

Ball::Ball(float startX, float startY, float r)
    : x(startX), y(startY), vx(0), vy(0),
      radius(r), active(true), lastHitBy(0),
      touchCount(0), lastTouchedTeam(0),
      textureFull(nullptr), textureBounce(nullptr),
      textureBounceHard(nullptr), textureRoll(nullptr),
      textureShot(nullptr), currentTexture(nullptr),
      renderer(nullptr),
      currentFrame(0), maxFrames(8), frameWidth(15), frameHeight(15), lastFrameTime(0) {
}

Ball::~Ball() {
    if (textureFull) SDL_DestroyTexture(textureFull);
    if (textureBounce) SDL_DestroyTexture(textureBounce);
    if (textureBounceHard) SDL_DestroyTexture(textureBounceHard);
    if (textureRoll) SDL_DestroyTexture(textureRoll);
    if (textureShot) SDL_DestroyTexture(textureShot);
}

void Ball::loadTextures(SDL_Renderer* ren) {
    renderer = ren;

    SDL_Surface* surface = IMG_Load("assets/ballFull.png");
    if (surface) {
        textureFull = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    } else {
        std::cerr << "Failed to load ballFull.png: " << IMG_GetError() << std::endl;
    }

    surface = IMG_Load("assets/ballBounce.png");
    if (surface) {
        textureBounce = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    surface = IMG_Load("assets/ballBounceHard.png");
    if (surface) {
        textureBounceHard = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    surface = IMG_Load("assets/ballRoll.png");
    if (surface) {
        textureRoll = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    surface = IMG_Load("assets/ballShot.png");
    if (surface) {
        textureShot = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    currentTexture = textureFull;
}

void Ball::update(float groundY) {
    if (!active) return;

    x += vx;
    y += vy;
    vy += GRAVITY * 0.6f;  // Decreased falling speed

    // Update texture based on velocity
    float speed = sqrt(vx * vx + vy * vy);
    if (speed > 12.0f) {
        currentTexture = textureShot ? textureShot : textureFull;
        maxFrames = 8;
    } else if (speed > 8.0f) {
        currentTexture = textureBounceHard ? textureBounceHard : textureFull;
        maxFrames = 8;
    } else if (speed > 3.0f) {
        currentTexture = textureBounce ? textureBounce : textureFull;
        maxFrames = 8;
    } else if (speed > 1.0f) {
        currentTexture = textureRoll ? textureRoll : textureFull;
        maxFrames = 8;
    } else {
        currentTexture = textureFull ? textureFull : currentTexture;
        maxFrames = 8;
    }

    // Update animation frame (50ms per frame for smooth ball rotation)
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastFrameTime >= 50) {
        currentFrame = (currentFrame + 1) % maxFrames;
        lastFrameTime = currentTime;
    }

    // Collision with walls
    if (x - radius < 0 || x + radius > 800) {  // SCREEN_WIDTH
        vx = -vx * BALL_BOUNCE;
        if (x - radius < 0) x = radius;
        if (x + radius > 800) x = 800 - radius;
    }

    // Collision with ground
    if (y + radius >= groundY) {
        active = false;
    }
}

void Ball::reset(int scoredBy, int screenWidth) {
    // Scorer serves next - ball drops on their side
    if (scoredBy == 1) {
        x = screenWidth / 4;  // Middle of left court
    } else if (scoredBy == 2) {
        x = screenWidth * 3 / 4;  // Middle of right court
    } else {
        x = screenWidth / 2;  // Center (default)
    }

    y = 100;
    vx = 0;
    vy = 0;
    active = true;
    lastHitBy = 0;
    touchCount = 0;
    lastTouchedTeam = 0;
}

bool Ball::checkCollision(Player& player, bool isPlayer1, int /* netX */) {
    SDL_FRect playerBounds = player.getBounds();
    float closestX = std::max(playerBounds.x, std::min(x, playerBounds.x + playerBounds.w));
    float closestY = std::max(playerBounds.y, std::min(y, playerBounds.y + playerBounds.h));

    float distanceX = x - closestX;
    float distanceY = y - closestY;
    float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

    if (distanceSquared < (radius * radius)) {
        int currentTeam = isPlayer1 ? 1 : 2;

        // If different team touched, reset count
        if (lastTouchedTeam != currentTeam) {
            touchCount = 0;
            lastTouchedTeam = currentTeam;
        }

        touchCount++;

        // Calculate bounce direction
        float dx = x - (playerBounds.x + playerBounds.w / 2);
        float dy = y - (playerBounds.y + playerBounds.h / 2);
        float distance = sqrt(dx * dx + dy * dy);

        if (distance > 0) {
            dx /= distance;
            dy /= distance;

            float force = 8.0f;
            vx = dx * force;
            vy = dy * force - 2.0f;

            // Move ball out of player
            x = closestX + dx * radius;
            y = closestY + dy * radius;

            lastHitBy = currentTeam;

            // Trigger player animation based on hit type
            float speed = sqrt(vx * vx + vy * vy);
            if (speed > 10.0f && dy < -0.5f) {
                player.setSmashing();  // Hard downward hit
            } else if (dy > 0.3f) {
                player.setReceiving();  // Ball coming from above
            } else {
                player.setBlocking();   // Normal hit
            }

            return true;
        }
    }
    return false;
}

void Ball::checkCrossedNet(int netX, int& previousSide) {
    int currentSide = (x < netX) ? 1 : 2;
    bool crossed = (previousSide != 0 && previousSide != currentSide);

    if (crossed) {
        // Reset touch count when ball crosses net
        touchCount = 0;
        lastTouchedTeam = 0;
    }

    previousSide = currentSide;
}

SDL_Rect Ball::getSrcRect() const {
    // Return source rectangle for current frame in spritesheet
    return {currentFrame * frameWidth, 5, frameWidth, frameHeight};
}

SDL_FRect Ball::getBounds() const {
    return {x - radius, y - radius, radius * 2, radius * 2};
}

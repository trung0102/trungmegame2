#include "../include/AIPlayer.h"
#include <cmath>
#include <cstdlib>

AIPlayer::AIPlayer(float startX, float startY, float diff)
    : Player(startX, startY), reactionSpeed(0.8f), difficulty(diff) {
}

bool AIPlayer::shouldMoveTowardsBall(const Ball& ball) const {
    // Only react if ball is active and on AI's side or coming towards AI
    if (!ball.isActive()) return false;

    // Add some randomness based on difficulty
    int chance = rand() % 100;
    if (chance > difficulty * 100) return false;  // Miss some opportunities

    return true;
}

bool AIPlayer::shouldJump(const Ball& ball) const {
    if (!ball.isActive() || !isOnGround()) return false;

    // Check if ball is at jumpable height and close enough
    float ballY = ball.getY();
    float ballX = ball.getX();
    float myX = getX();

    // Jump if ball is high and close
    bool ballIsHigh = ballY < getY() - 20;
    bool ballIsClose = std::abs(ballX - myX) < 80;

    // AI difficulty affects jump timing
    int chance = rand() % 100;
    return ballIsHigh && ballIsClose && (chance < difficulty * 80);
}

float AIPlayer::calculateTargetX(const Ball& ball) const {
    // Predict where ball will be
    float ballX = ball.getX();

    // Simple prediction: move to ball's X position
    float targetX = ballX;

    // Add some imperfection based on difficulty
    float error = (1.0f - difficulty) * 50.0f;
    targetX += ((rand() % 100) - 50) * error / 100.0f;

    return targetX;
}

void AIPlayer::autoPlay(const Ball& ball, int netX) {
    if (!shouldMoveTowardsBall(ball)) {
        setVelocityX(0);
        return;
    }

    // Calculate where AI should move
    float targetX = calculateTargetX(ball);
    float myX = getX() + getWidth() / 2;  // Center of AI player

    // Move towards target
    float distance = targetX - myX;

    if (std::abs(distance) > 15) {  // Dead zone
        if (distance > 0) {
            setVelocityX(PLAYER_SPEED * reactionSpeed);
        } else {
            setVelocityX(-PLAYER_SPEED * reactionSpeed);
        }
    } else {
        setVelocityX(0);
    }

    // Decide whether to jump
    if (shouldJump(ball)) {
        jump();
    }

    // Stay on AI's side of court
    if (getX() < netX && getX() + getWidth() >= netX) {
        // AI on left side, don't cross net
        setVelocityX(-PLAYER_SPEED * 0.5f);
    } else if (getX() > netX && getX() <= netX) {
        // AI on right side, don't cross net
        setVelocityX(PLAYER_SPEED * 0.5f);
    }
}

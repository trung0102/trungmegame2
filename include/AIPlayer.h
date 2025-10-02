#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "Player.h"
#include "Ball.h"

class AIPlayer : public Player {
private:
    float reactionSpeed;
    float difficulty;  // 0.0 - 1.0, higher = better AI

    // AI decision making
    bool shouldMoveTowardsBall(const Ball& ball) const;
    bool shouldJump(const Ball& ball) const;
    float calculateTargetX(const Ball& ball) const;

public:
    AIPlayer(float startX, float startY, float diff = 0.7f);

    // AI control instead of keyboard
    void autoPlay(const Ball& ball, int netX);

    void setDifficulty(float diff) { difficulty = diff; }
    float getDifficulty() const { return difficulty; }
};

#endif // AIPLAYER_H

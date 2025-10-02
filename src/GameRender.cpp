#include "../include/Game.h"
#include <cmath>

void Game::render() {
    if (currentMode == GameMode::MENU) {
        renderMenu();
        SDL_RenderPresent(renderer);
        return;
    }

    // Draw background image
    if (backgroundTexture) {
        SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &bgRect);
    } else {
        // Fallback to colored background
        SDL_SetRenderDrawColor(renderer, COLOR_SKY.r, COLOR_SKY.g, COLOR_SKY.b, COLOR_SKY.a);
        SDL_Rect skyRect = {0, 0, SCREEN_WIDTH, (int)GROUND_Y};
        SDL_RenderFillRect(renderer, &skyRect);

        SDL_SetRenderDrawColor(renderer, COLOR_SAND.r, COLOR_SAND.g, COLOR_SAND.b, COLOR_SAND.a);
        SDL_Rect sandRect = {0, (int)GROUND_Y, SCREEN_WIDTH, SCREEN_HEIGHT - (int)GROUND_Y};
        SDL_RenderFillRect(renderer, &sandRect);
    }

    // Draw court lines (black color)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Draw ground line (horizontal line at ground level)
    SDL_Rect groundLine = {0, (int)GROUND_Y, SCREEN_WIDTH, 3};
    SDL_RenderFillRect(renderer, &groundLine);

    // Draw center line (vertical line in middle)
    SDL_Rect centerLine = {NET_X - 1, (int)GROUND_Y, 2, SCREEN_HEIGHT - (int)GROUND_Y};
    SDL_RenderFillRect(renderer, &centerLine);

    // Draw net (black color on top of center line)
    // Net post (thicker at bottom)
    SDL_Rect netPost = {NET_X - 3, (int)GROUND_Y - 100, 6, 100};
    SDL_RenderFillRect(renderer, &netPost);

    // Net mesh pattern (horizontal lines)
    for (int i = 0; i < 10; i++) {
        int y = (int)GROUND_Y - 95 + (i * 10);
        SDL_Rect meshLine = {NET_X - 20, y, 40, 1};
        SDL_RenderFillRect(renderer, &meshLine);
    }

    // Vertical mesh lines
    for (int i = -2; i <= 2; i++) {
        if (i == 0) continue; // Skip center
        int x = NET_X + (i * 10);
        SDL_Rect meshLine = {x, (int)GROUND_Y - 95, 1, 95};
        SDL_RenderFillRect(renderer, &meshLine);
    }

    renderPlayers();
    renderBall();
    renderScore();

    if (gameOver) {
        renderGameOver();
    }

    if (isCountingDown) {
        renderCountdown();
    }

    if (isPaused) {
        renderPause();
    }

    SDL_RenderPresent(renderer);
}

void Game::renderMenu() {
    // Draw background
    SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255);
    SDL_RenderClear(renderer);

    // Draw title
    SDL_Color titleColor = {255, 215, 0, 255};  // Gold
    renderText("BEACH VOLLEYBALL", SCREEN_WIDTH/2 - 200, 80, titleColor, titleFont);

    // Draw option 1: Player vs Player
    SDL_Color option1Color;
    if (menuSelection == 0) {
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);  // Highlighted
        option1Color = {0, 0, 0, 255};  // Black text on gold background
    } else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);  // Not selected
        option1Color = {255, 255, 255, 255};  // White text on gray background
    }
    SDL_Rect option1Box = {SCREEN_WIDTH/2 - 200, 220, 400, 70};
    SDL_RenderFillRect(renderer, &option1Box);

    // Draw option 1 border
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < 3; i++) {
        SDL_Rect border = {SCREEN_WIDTH/2 - 200 - i, 220 - i, 400 + 2*i, 70 + 2*i};
        SDL_RenderDrawRect(renderer, &border);
    }
    renderText("Player vs Player", SCREEN_WIDTH/2 - 120, 240, option1Color, font);

    // Draw option 2: Player vs AI
    SDL_Color option2Color;
    if (menuSelection == 1) {
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);  // Highlighted
        option2Color = {0, 0, 0, 255};  // Black text on gold background
    } else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);  // Not selected
        option2Color = {255, 255, 255, 255};  // White text on gray background
    }
    SDL_Rect option2Box = {SCREEN_WIDTH/2 - 200, 320, 400, 70};
    SDL_RenderFillRect(renderer, &option2Box);

    // Draw option 2 border
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < 3; i++) {
        SDL_Rect border = {SCREEN_WIDTH/2 - 200 - i, 320 - i, 400 + 2*i, 70 + 2*i};
        SDL_RenderDrawRect(renderer, &border);
    }
    renderText("Player vs AI", SCREEN_WIDTH/2 - 90, 340, option2Color, font);

    // Draw instructions
    SDL_Color instructColor = {200, 200, 200, 255};
    renderText("Use UP/DOWN arrows to select", SCREEN_WIDTH/2 - 150, 450, instructColor, font);
    renderText("Press SPACE to confirm", SCREEN_WIDTH/2 - 120, 490, instructColor, font);
}

void Game::renderText(const char* text, int x, int y, SDL_Color color, TTF_Font* textFont) {
    if (!textFont) textFont = font;
    if (!textFont) return;  // No font loaded

    SDL_Surface* textSurface = TTF_RenderText_Blended(textFont, text, color);
    if (!textSurface) return;

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture) {
        SDL_Rect destRect = {x, y, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &destRect);
        SDL_DestroyTexture(textTexture);
    }
    SDL_FreeSurface(textSurface);
}

void Game::drawText(int x, int y, const char* text, SDL_Color color, TTF_Font* textFont) {
    renderText(text, x, y, color, textFont);
}

void Game::renderPlayers() {
    // Draw player 1
    if (player1) {
        SDL_Texture* texture = player1->getTexture();
        if (texture) {
            SDL_Rect srcRect = player1->getSrcRect();
            SDL_Rect destRect = {
                (int)player1->getX(), (int)player1->getY(),
                (int)player1->getWidth(), (int)player1->getHeight()
            };
            // Flip texture if facing left
            SDL_RendererFlip flip = player1->isFacingRight() ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, 0, NULL, flip);
        } else {
            // Fallback to colored rectangle
            SDL_SetRenderDrawColor(renderer, COLOR_PLAYER1.r, COLOR_PLAYER1.g, COLOR_PLAYER1.b, COLOR_PLAYER1.a);
            SDL_Rect p1Rect = {
                (int)player1->getX(), (int)player1->getY(),
                (int)player1->getWidth(), (int)player1->getHeight()
            };
            SDL_RenderFillRect(renderer, &p1Rect);
        }
    }

    // Draw player 2 or AI player
    if (currentMode == GameMode::PLAYER_VS_AI && aiPlayer) {
        SDL_Texture* texture = aiPlayer->getTexture();
        if (texture) {
            SDL_Rect srcRect = aiPlayer->getSrcRect();
            SDL_Rect destRect = {
                (int)aiPlayer->getX(), (int)aiPlayer->getY(),
                (int)aiPlayer->getWidth(), (int)aiPlayer->getHeight()
            };
            SDL_RendererFlip flip = aiPlayer->isFacingRight() ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, 0, NULL, flip);
        } else {
            SDL_SetRenderDrawColor(renderer, COLOR_PLAYER2.r, COLOR_PLAYER2.g, COLOR_PLAYER2.b, COLOR_PLAYER2.a);
            SDL_Rect p2Rect = {
                (int)aiPlayer->getX(), (int)aiPlayer->getY(),
                (int)aiPlayer->getWidth(), (int)aiPlayer->getHeight()
            };
            SDL_RenderFillRect(renderer, &p2Rect);
        }
    } else if (currentMode == GameMode::PLAYER_VS_PLAYER && player2) {
        SDL_Texture* texture = player2->getTexture();
        if (texture) {
            SDL_Rect srcRect = player2->getSrcRect();
            SDL_Rect destRect = {
                (int)player2->getX(), (int)player2->getY(),
                (int)player2->getWidth(), (int)player2->getHeight()
            };
            SDL_RendererFlip flip = player2->isFacingRight() ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, 0, NULL, flip);
        } else {
            SDL_SetRenderDrawColor(renderer, COLOR_PLAYER2.r, COLOR_PLAYER2.g, COLOR_PLAYER2.b, COLOR_PLAYER2.a);
            SDL_Rect p2Rect = {
                (int)player2->getX(), (int)player2->getY(),
                (int)player2->getWidth(), (int)player2->getHeight()
            };
            SDL_RenderFillRect(renderer, &p2Rect);
        }
    }
}

void Game::renderBall() {
    if (!ball || !ball->isActive()) return;

    SDL_Texture* texture = ball->getTexture();
    if (texture) {
        // Render texture with animation frame
        SDL_Rect srcRect = ball->getSrcRect();
        float radius = ball->getRadius();
        float size = radius * 2;
        SDL_Rect destRect = {
            (int)(ball->getX() - radius),
            (int)(ball->getY() - radius),
            (int)size,
            (int)size
        };
        SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
    } else {
        // Fallback: Draw circle if texture not available
        SDL_SetRenderDrawColor(renderer, COLOR_BALL.r, COLOR_BALL.g, COLOR_BALL.b, COLOR_BALL.a);
        float radius = ball->getRadius();
        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius * 2; h++) {
                int dx = radius - w;
                int dy = radius - h;
                if ((dx*dx + dy*dy) <= (radius * radius)) {
                    SDL_RenderDrawPoint(renderer, ball->getX() + dx, ball->getY() + dy);
                }
            }
        }
    }
}

void Game::renderScore() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Player 1 score (left)
    if (player1) {
        drawNumber(150, 30, player1->getScore());
    }

    // Player 2 or AI score (right)
    if (currentMode == GameMode::PLAYER_VS_AI && aiPlayer) {
        drawNumber(650, 30, aiPlayer->getScore());
    } else if (currentMode == GameMode::PLAYER_VS_PLAYER && player2) {
        drawNumber(650, 30, player2->getScore());
    }
}

void Game::drawNumber(int x, int y, int num, int size) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // 7-segment display
    bool seg[7] = {false};

    switch(num % 100) {  // Handle numbers 0-99
        case 0: seg[0]=seg[1]=seg[2]=seg[3]=seg[4]=seg[5]=true; break;
        case 1: seg[1]=seg[2]=true; break;
        case 2: seg[0]=seg[1]=seg[3]=seg[4]=seg[6]=true; break;
        case 3: seg[0]=seg[1]=seg[2]=seg[3]=seg[6]=true; break;
        case 4: seg[1]=seg[2]=seg[5]=seg[6]=true; break;
        case 5: seg[0]=seg[2]=seg[3]=seg[5]=seg[6]=true; break;
        case 6: seg[0]=seg[2]=seg[3]=seg[4]=seg[5]=seg[6]=true; break;
        case 7: seg[0]=seg[1]=seg[2]=true; break;
        case 8: seg[0]=seg[1]=seg[2]=seg[3]=seg[4]=seg[5]=seg[6]=true; break;
        case 9: seg[0]=seg[1]=seg[2]=seg[3]=seg[5]=seg[6]=true; break;
        // Handle 10-15
        default:
            if (num >= 10) {
                drawNumber(x - 35, y, 1, size);  // Draw '1' for tens
                drawNumber(x, y, num % 10, size);  // Draw ones digit
                return;
            }
    }

    // Draw segments
    if (seg[0]) { SDL_Rect r = {x, y, size, 5}; SDL_RenderFillRect(renderer, &r); }
    if (seg[1]) { SDL_Rect r = {x+size-5, y, 5, size/2}; SDL_RenderFillRect(renderer, &r); }
    if (seg[2]) { SDL_Rect r = {x+size-5, y+size/2, 5, size/2}; SDL_RenderFillRect(renderer, &r); }
    if (seg[3]) { SDL_Rect r = {x, y+size-5, size, 5}; SDL_RenderFillRect(renderer, &r); }
    if (seg[4]) { SDL_Rect r = {x, y+size/2, 5, size/2}; SDL_RenderFillRect(renderer, &r); }
    if (seg[5]) { SDL_Rect r = {x, y, 5, size/2}; SDL_RenderFillRect(renderer, &r); }
    if (seg[6]) { SDL_Rect r = {x, y+size/2-2, size, 5}; SDL_RenderFillRect(renderer, &r); }
}

void Game::drawBigNumber(int x, int y, int num, int size) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    bool seg[7] = {false};

    switch(num) {
        case 0: seg[0]=seg[1]=seg[2]=seg[3]=seg[4]=seg[5]=true; break;
        case 1: seg[1]=seg[2]=true; break;
        case 2: seg[0]=seg[1]=seg[3]=seg[4]=seg[6]=true; break;
        case 3: seg[0]=seg[1]=seg[2]=seg[3]=seg[6]=true; break;
        case 4: seg[1]=seg[2]=seg[5]=seg[6]=true; break;
        case 5: seg[0]=seg[2]=seg[3]=seg[5]=seg[6]=true; break;
        case 6: seg[0]=seg[2]=seg[3]=seg[4]=seg[5]=seg[6]=true; break;
        case 7: seg[0]=seg[1]=seg[2]=true; break;
        case 8: seg[0]=seg[1]=seg[2]=seg[3]=seg[4]=seg[5]=seg[6]=true; break;
        case 9: seg[0]=seg[1]=seg[2]=seg[3]=seg[5]=seg[6]=true; break;
    }

    int thickness = 10;
    if (seg[0]) { SDL_Rect r = {x, y, size, thickness}; SDL_RenderFillRect(renderer, &r); }
    if (seg[1]) { SDL_Rect r = {x+size-thickness, y, thickness, size/2}; SDL_RenderFillRect(renderer, &r); }
    if (seg[2]) { SDL_Rect r = {x+size-thickness, y+size/2, thickness, size/2}; SDL_RenderFillRect(renderer, &r); }
    if (seg[3]) { SDL_Rect r = {x, y+size-thickness, size, thickness}; SDL_RenderFillRect(renderer, &r); }
    if (seg[4]) { SDL_Rect r = {x, y+size/2, thickness, size/2}; SDL_RenderFillRect(renderer, &r); }
    if (seg[5]) { SDL_Rect r = {x, y, thickness, size/2}; SDL_RenderFillRect(renderer, &r); }
    if (seg[6]) { SDL_Rect r = {x, y+size/2-thickness/2, size, thickness}; SDL_RenderFillRect(renderer, &r); }
}

void Game::renderCountdown() {
    // Draw dark overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);

    // Draw big countdown number
    if (countdown > 0) {
        int centerX = SCREEN_WIDTH / 2 - 40;
        int centerY = SCREEN_HEIGHT / 2 - 40;
        drawBigNumber(centerX, centerY, countdown, 80);
    }
}

void Game::renderPause() {
    // Draw dark overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);

    // Draw pause bars
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect bar1 = {SCREEN_WIDTH/2 - 30, SCREEN_HEIGHT/2 - 40, 20, 80};
    SDL_Rect bar2 = {SCREEN_WIDTH/2 + 10, SCREEN_HEIGHT/2 - 40, 20, 80};
    SDL_RenderFillRect(renderer, &bar1);
    SDL_RenderFillRect(renderer, &bar2);
}

void Game::renderGameOver() {
    // Draw dark overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);

    // Draw winner box
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect winnerBox = {SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 50, 300, 100};
    SDL_RenderFillRect(renderer, &winnerBox);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect textBox = {SCREEN_WIDTH/2 - 140, SCREEN_HEIGHT/2 - 40, 280, 80};
    SDL_RenderFillRect(renderer, &textBox);
}

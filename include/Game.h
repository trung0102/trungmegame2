#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "Player.h"
#include "AIPlayer.h"
#include "Ball.h"

enum class GameMode {
    MENU,
    PLAYER_VS_AI,
    PLAYER_VS_PLAYER
};

enum class PlayerControls {
    WASD,
    ARROWS
};

class Game {
private:
    // SDL components
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;

    // Game objects
    Player* player1;
    Player* player2;
    AIPlayer* aiPlayer;
    Ball* ball;

    // Game state
    GameMode currentMode;
    PlayerControls player1Controls;  // For single player mode
    bool gameOver;
    bool isPaused;
    bool isCountingDown;
    int winner;
    int countdown;
    Uint32 countdownStartTime;
    int ballSide;

    // Menu state
    int menuSelection;  // 0 = Player vs Player, 1 = Player vs AI

    // Audio
    Mix_Chunk* hitBallSound;
    Mix_Chunk* fallBallSound;
    Mix_Chunk* getPointSound;
    Mix_Music* backgroundMusic;

    // Graphics
    SDL_Texture* backgroundTexture;
    TTF_Font* font;
    TTF_Font* titleFont;

    // Constants
    static constexpr int SCREEN_WIDTH = 800;
    static constexpr int SCREEN_HEIGHT = 600;
    static constexpr int WINNING_SCORE = 15;
    static constexpr float GROUND_Y = 550.0f;
    static constexpr int NET_X = 400;

    // Colors
    struct Color {
        Uint8 r, g, b, a;
    };
    static constexpr Color COLOR_SKY = {25, 25, 50, 255};        // Xanh đen (trời đêm)
    static constexpr Color COLOR_SAND = {194, 142, 84, 255};     // Nâu cát đậm
    static constexpr Color COLOR_NET = {255, 255, 255, 255};
    static constexpr Color COLOR_PLAYER1 = {255, 100, 100, 255};
    static constexpr Color COLOR_PLAYER2 = {100, 100, 255, 255};
    static constexpr Color COLOR_BALL = {255, 255, 0, 255};

    // Private methods
    bool initSDL();
    bool loadAudio();
    void handleEvents();
    void handleMenuEvents();
    void handleGameEvents();
    void update();
    void render();
    void renderMenu();
    void renderPlayers();
    void renderBall();
    void renderScore();
    void renderCountdown();
    void renderPause();
    void renderGameOver();
    void drawNumber(int x, int y, int num, int size = 30);
    void drawBigNumber(int x, int y, int num, int size = 80);
    void drawText(int x, int y, const char* text, SDL_Color color, TTF_Font* textFont = nullptr);
    void renderText(const char* text, int x, int y, SDL_Color color, TTF_Font* textFont = nullptr);

    void startCountdown();
    void updateCountdown();
    void resetPlayersPosition();
    void handleScoring(int scoringPlayer);
    void startGame(GameMode mode, PlayerControls controls = PlayerControls::WASD);
    void resetGame();

public:
    Game();
    ~Game();

    bool init();
    void run();
    void clean();
};

#endif // GAME_H

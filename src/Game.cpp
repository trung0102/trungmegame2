#include "../include/Game.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cmath>

Game::Game()
    : window(nullptr), renderer(nullptr), running(true),
      player1(nullptr), player2(nullptr), aiPlayer(nullptr), ball(nullptr),
      currentMode(GameMode::MENU), player1Controls(PlayerControls::WASD),
      gameOver(false), isPaused(false), isCountingDown(true),
      winner(0), countdown(3), countdownStartTime(0), ballSide(0),
      menuSelection(0),
      hitBallSound(nullptr), fallBallSound(nullptr),
      getPointSound(nullptr), backgroundMusic(nullptr),
      backgroundTexture(nullptr), font(nullptr), titleFont(nullptr) {
}

Game::~Game() {
    clean();
}

bool Game::init() {
    if (!initSDL()) return false;
    if (!loadAudio()) {
        std::cerr << "Warning: Audio could not be loaded. Game will run without sound." << std::endl;
    }

    // Load background texture
    SDL_Surface* bgSurface = IMG_Load("assets/beachbkgO.png");
    if (bgSurface) {
        backgroundTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
        SDL_FreeSurface(bgSurface);
    } else {
        std::cerr << "Warning: Could not load background image." << std::endl;
    }

    // Game objects will be created when game mode is selected
    return true;
}

bool Game::initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Beach Volleyball",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Load fonts (using system default font or embedded)
    // Try to load a common system font
    font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 24);
    if (!font) {
        // Try alternative paths
        font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
    }
    if (!font) {
        std::cerr << "Warning: Could not load font! TTF_Error: " << TTF_GetError() << std::endl;
    }

    titleFont = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 48);
    if (!titleFont) {
        titleFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 48);
    }
    if (!titleFont) {
        std::cerr << "Warning: Could not load title font! TTF_Error: " << TTF_GetError() << std::endl;
    }

    return true;
}

bool Game::loadAudio() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    hitBallSound = Mix_LoadWAV("assets/sounds/hitball.wav");
    fallBallSound = Mix_LoadWAV("assets/sounds/fallball.wav");
    getPointSound = Mix_LoadWAV("assets/sounds/getpoint.wav");
    backgroundMusic = Mix_LoadMUS("assets/sounds/background.mp3");

    if (backgroundMusic) {
        Mix_PlayMusic(backgroundMusic, -1);
        Mix_VolumeMusic(32);
    }

    return true;
}

void Game::handleEvents() {
    if (currentMode == GameMode::MENU) {
        handleMenuEvents();
    } else {
        handleGameEvents();
    }
}

void Game::handleMenuEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            return;
        }

        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_UP) {
                // Move selection up
                menuSelection = (menuSelection - 1 + 2) % 2;  // 2 options
                std::cout << "Menu selection: " << menuSelection << std::endl;
            } else if (e.key.keysym.sym == SDLK_DOWN) {
                // Move selection down
                menuSelection = (menuSelection + 1) % 2;
                std::cout << "Menu selection: " << menuSelection << std::endl;
            } else if (e.key.keysym.sym == SDLK_SPACE) {
                // Confirm selection
                if (menuSelection == 0) {
                    // Player vs Player
                    std::cout << "Player vs Player selected!" << std::endl;
                    startGame(GameMode::PLAYER_VS_PLAYER);
                } else if (menuSelection == 1) {
                    // Player vs AI - default to WASD controls
                    std::cout << "Player vs AI selected!" << std::endl;
                    startGame(GameMode::PLAYER_VS_AI, PlayerControls::WASD);
                }
            }
        }
    }
}

void Game::handleGameEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }

        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_SPACE) {
                if (gameOver) {
                    resetGame();
                } else if (!isCountingDown) {
                    isPaused = !isPaused;
                }
            } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                // Return to menu
                resetGame();
                currentMode = GameMode::MENU;
            }
        }
    }

    if (gameOver || isCountingDown || isPaused) return;

    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    if (currentMode == GameMode::PLAYER_VS_AI) {
        // Player controls based on selected scheme
        player1->setVelocityX(0);
        if (player1Controls == PlayerControls::WASD) {
            if (keystate[SDL_SCANCODE_A]) player1->setVelocityX(-Player::PLAYER_SPEED);
            if (keystate[SDL_SCANCODE_D]) player1->setVelocityX(Player::PLAYER_SPEED);
            if (keystate[SDL_SCANCODE_W]) player1->jump();
        } else {
            if (keystate[SDL_SCANCODE_LEFT]) player1->setVelocityX(-Player::PLAYER_SPEED);
            if (keystate[SDL_SCANCODE_RIGHT]) player1->setVelocityX(Player::PLAYER_SPEED);
            if (keystate[SDL_SCANCODE_UP]) player1->jump();
        }

        // AI controls player 2
        if (aiPlayer) {
            aiPlayer->autoPlay(*ball, NET_X);
        }
    } else if (currentMode == GameMode::PLAYER_VS_PLAYER) {
        // Player 1 controls (WASD)
        player1->setVelocityX(0);
        if (keystate[SDL_SCANCODE_A]) player1->setVelocityX(-Player::PLAYER_SPEED);
        if (keystate[SDL_SCANCODE_D]) player1->setVelocityX(Player::PLAYER_SPEED);
        if (keystate[SDL_SCANCODE_W]) player1->jump();

        // Player 2 controls (Arrow keys)
        player2->setVelocityX(0);
        if (keystate[SDL_SCANCODE_LEFT]) player2->setVelocityX(-Player::PLAYER_SPEED);
        if (keystate[SDL_SCANCODE_RIGHT]) player2->setVelocityX(Player::PLAYER_SPEED);
        if (keystate[SDL_SCANCODE_UP]) player2->jump();
    }
}

void Game::startCountdown() {
    countdown = 3;
    countdownStartTime = SDL_GetTicks();
    isCountingDown = true;
    ballSide = 0;
}

void Game::updateCountdown() {
    if (!isCountingDown) return;

    Uint32 currentTime = SDL_GetTicks();
    Uint32 elapsed = currentTime - countdownStartTime;

    if (elapsed >= 1000) {
        countdown--;
        countdownStartTime = currentTime;

        if (countdown <= 0) {
            isCountingDown = false;
        }
    }
}

void Game::resetPlayersPosition() {
    if (player1) player1->resetPosition();

    if (currentMode == GameMode::PLAYER_VS_AI && aiPlayer) {
        aiPlayer->resetPosition();
    } else if (currentMode == GameMode::PLAYER_VS_PLAYER && player2) {
        player2->resetPosition();
    }
}

void Game::startGame(GameMode mode, PlayerControls controls) {
    currentMode = mode;
    player1Controls = controls;

    // Clean up existing objects
    if (player1) delete player1;
    if (player2) delete player2;
    if (aiPlayer) delete aiPlayer;
    if (ball) delete ball;

    player1 = nullptr;
    player2 = nullptr;
    aiPlayer = nullptr;

    // Create new game objects based on mode
    if (mode == GameMode::PLAYER_VS_AI) {
        player1 = new Player(100, 400);
        player1->loadTextures(renderer, false);
        player1->setBoundaries(0, NET_X);  // Left court only

        aiPlayer = new AIPlayer(650, 400, 0.7f);  // Medium difficulty
        aiPlayer->loadTextures(renderer, true);
        aiPlayer->setBoundaries(NET_X, SCREEN_WIDTH);  // Right court only

        ball = new Ball(SCREEN_WIDTH / 2, 100);
        ball->loadTextures(renderer);

        std::cout << "=== BEACH VOLLEYBALL - PLAYER VS AI ===" << std::endl;
        if (controls == PlayerControls::WASD) {
            std::cout << "Controls: A/D to move, W to jump" << std::endl;
        } else {
            std::cout << "Controls: Arrow keys to move/jump" << std::endl;
        }
    } else if (mode == GameMode::PLAYER_VS_PLAYER) {
        player1 = new Player(100, 400);
        player1->loadTextures(renderer, false);
        player1->setBoundaries(0, NET_X);  // Left court only

        player2 = new Player(650, 400);
        player2->loadTextures(renderer, true);
        player2->setBoundaries(NET_X, SCREEN_WIDTH);  // Right court only

        ball = new Ball(SCREEN_WIDTH / 2, 100);
        ball->loadTextures(renderer);

        std::cout << "=== BEACH VOLLEYBALL - PLAYER VS PLAYER ===" << std::endl;
        std::cout << "Player 1 (Red): A/D to move, W to jump" << std::endl;
        std::cout << "Player 2 (Blue): Arrow keys to move/jump" << std::endl;
    }

    std::cout << "Space: Pause/Resume" << std::endl;
    std::cout << "ESC: Return to menu" << std::endl;
    std::cout << "First to " << WINNING_SCORE << " points wins!" << std::endl;
    std::cout << "Max 3 touches per side!" << std::endl;

    gameOver = false;
    isPaused = false;
    winner = 0;
    startCountdown();
}

void Game::resetGame() {
    if (player1) player1->resetScore();
    if (player2) player2->resetScore();
    if (aiPlayer) aiPlayer->resetScore();

    gameOver = false;
    isPaused = false;
    winner = 0;

    if (ball) ball->reset();
    resetPlayersPosition();
    startCountdown();
}

void Game::handleScoring(int scoringPlayer) {
    if (scoringPlayer == 1) {
        player1->addScore();
    } else if (scoringPlayer == 2) {
        if (currentMode == GameMode::PLAYER_VS_AI && aiPlayer) {
            aiPlayer->addScore();
        } else if (player2) {
            player2->addScore();
        }
    }

    if (getPointSound) Mix_PlayChannel(-1, getPointSound, 0);

    int score1 = player1 ? player1->getScore() : 0;
    int score2 = (currentMode == GameMode::PLAYER_VS_AI && aiPlayer) ? aiPlayer->getScore() : (player2 ? player2->getScore() : 0);

    std::cout << "Score: " << score1 << " - " << score2 << std::endl;

    // Check for game over
    if (score1 >= WINNING_SCORE) {
        gameOver = true;
        winner = 1;
        std::cout << "=======================" << std::endl;
        std::cout << "PLAYER 1 WINS!" << std::endl;
        std::cout << "Final Score: " << score1 << " - " << score2 << std::endl;
        std::cout << "Press SPACE to restart or ESC for menu" << std::endl;
        std::cout << "=======================" << std::endl;
    } else if (score2 >= WINNING_SCORE) {
        gameOver = true;
        winner = 2;
        std::cout << "=======================" << std::endl;
        if (currentMode == GameMode::PLAYER_VS_AI) {
            std::cout << "AI WINS!" << std::endl;
        } else {
            std::cout << "PLAYER 2 WINS!" << std::endl;
        }
        std::cout << "Final Score: " << score1 << " - " << score2 << std::endl;
        std::cout << "Press SPACE to restart or ESC for menu" << std::endl;
        std::cout << "=======================" << std::endl;
    } else {
        ball->reset(scoringPlayer, SCREEN_WIDTH);
        resetPlayersPosition();
        startCountdown();
    }
}

void Game::update() {
    if (currentMode == GameMode::MENU) return;
    if (gameOver || isPaused) return;

    if (isCountingDown) {
        updateCountdown();
        return;
    }

    // Update players based on mode
    if (player1) player1->update(GROUND_Y);

    if (currentMode == GameMode::PLAYER_VS_AI) {
        if (aiPlayer) aiPlayer->update(GROUND_Y);
    } else if (currentMode == GameMode::PLAYER_VS_PLAYER) {
        if (player2) player2->update(GROUND_Y);
    }

    if (ball && ball->isActive()) {
        ball->update(GROUND_Y);
        ball->checkCrossedNet(NET_X, ballSide);

        // Check 3-touch violation
        if (ball->checkTouchViolation()) {
            if (fallBallSound) Mix_PlayChannel(-1, fallBallSound, 0);

            int scoringPlayer = (ball->getLastTouchedTeam() == 1) ? 2 : 1;
            std::cout << "Team " << ball->getLastTouchedTeam()
                     << " violated 3-touch rule! Team " << scoringPlayer << " scores!" << std::endl;

            handleScoring(scoringPlayer);
            return;
        }

        // Check collisions
        bool hit1 = false;
        bool hit2 = false;

        if (player1) {
            hit1 = ball->checkCollision(*player1, true, NET_X);
        }

        if (currentMode == GameMode::PLAYER_VS_AI && aiPlayer) {
            hit2 = ball->checkCollision(*aiPlayer, false, NET_X);
        } else if (currentMode == GameMode::PLAYER_VS_PLAYER && player2) {
            hit2 = ball->checkCollision(*player2, false, NET_X);
        }

        if (hit1 || hit2) {
            if (hitBallSound) Mix_PlayChannel(-1, hitBallSound, 0);

            // Check for violation after hit
            if (ball->checkTouchViolation()) {
                if (fallBallSound) Mix_PlayChannel(-1, fallBallSound, 0);
                int scoringPlayer = (ball->getLastTouchedTeam() == 1) ? 2 : 1;
                handleScoring(scoringPlayer);
                return;
            }
        }

        // Check if ball hits ground
        if (!ball->isActive()) {
            if (fallBallSound) Mix_PlayChannel(-1, fallBallSound, 0);

            int scoringPlayer = 0;
            if (ball->getX() < NET_X) {
                scoringPlayer = 2;  // Ball on left, team 2 scores
                if (currentMode == GameMode::PLAYER_VS_AI) {
                    std::cout << "AI SCORES!" << std::endl;
                } else {
                    std::cout << "Player 2 SCORES!" << std::endl;
                }
            } else {
                scoringPlayer = 1;  // Ball on right, player 1 scores
                std::cout << "Player 1 SCORES!" << std::endl;
            }

            handleScoring(scoringPlayer);
        }
    }
}

void Game::run() {
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;

    Uint32 frameStart;
    int frameTime;

    std::cout << "=== BEACH VOLLEYBALL ===" << std::endl;
    std::cout << "Use UP/DOWN arrows to select game mode" << std::endl;
    std::cout << "Press SPACE to confirm selection" << std::endl;

    while (running) {
        frameStart = SDL_GetTicks();

        handleEvents();
        update();

        SDL_RenderClear(renderer);
        render();

        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void Game::clean() {
    if (player1) delete player1;
    if (player2) delete player2;
    if (aiPlayer) delete aiPlayer;
    if (ball) delete ball;

    if (hitBallSound) Mix_FreeChunk(hitBallSound);
    if (fallBallSound) Mix_FreeChunk(fallBallSound);
    if (getPointSound) Mix_FreeChunk(getPointSound);
    if (backgroundMusic) Mix_FreeMusic(backgroundMusic);

    if (backgroundTexture) SDL_DestroyTexture(backgroundTexture);

    if (font) TTF_CloseFont(font);
    if (titleFont) TTF_CloseFont(titleFont);

    Mix_CloseAudio();
    TTF_Quit();

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    SDL_Quit();
}

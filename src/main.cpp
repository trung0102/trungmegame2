#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <cmath>
#include <string>

// Kích thước màn hình
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Màu sắc
const SDL_Color COLOR_SKY = {135, 206, 235, 255};
const SDL_Color COLOR_SAND = {238, 214, 175, 255};
const SDL_Color COLOR_NET = {255, 255, 255, 255};
const SDL_Color COLOR_PLAYER1 = {255, 100, 100, 255};
const SDL_Color COLOR_PLAYER2 = {100, 100, 255, 255};
const SDL_Color COLOR_BALL = {255, 255, 0, 255};

// Hằng số game
const float GRAVITY = 0.2f;
const float PLAYER_SPEED = 5.0f;
const float PLAYER_JUMP = -12.0f;
const float BALL_BOUNCE = 0.8f;
const int WINNING_SCORE = 15;

// Struct cho người chơi
struct Player {
    float x, y;
    float vx, vy;
    float width, height;
    bool onGround;
    int score;
    float initialX, initialY; // Vị trí ban đầu

    Player(float startX, float startY) : x(startX), y(startY), vx(0), vy(0),
                                          width(30), height(60), onGround(false), score(0),
                                          initialX(startX), initialY(startY) {}

    void update(float groundY) {
        x += vx;
        y += vy;
        vy += GRAVITY;

        // Kiểm tra va chạm với mặt đất
        if (y + height >= groundY) {
            y = groundY - height;
            vy = 0;
            onGround = true;
        } else {
            onGround = false;
        }

        // Giới hạn trong màn hình
        if (x < 0) x = 0;
        if (x + width > SCREEN_WIDTH) x = SCREEN_WIDTH - width;
    }

    void jump() {
        if (onGround) {
            vy = PLAYER_JUMP;
            onGround = false;
        }
    }

    void resetPosition() {
        x = initialX;
        y = initialY;
        vx = 0;
        vy = 0;
    }
};

// Struct cho bóng
struct Ball {
    float x, y;
    float vx, vy;
    float radius;
    bool active;
    int lastHitBy; // 0: không ai, 1: player1, 2: player2
    int touchCount; // Số lần chạm của đội hiện tại
    int lastTouchedTeam; // 1: team1, 2: team2

    Ball() : x(SCREEN_WIDTH / 2), y(100), vx(0), vy(0),
             radius(15), active(true), lastHitBy(0), touchCount(0), lastTouchedTeam(0) {}

    void reset(int scoredBy = 0) {
        // Nếu player 1 ghi điểm (bóng rơi bên phải), bóng rơi ở sân trái (player 1)
        // Nếu player 2 ghi điểm (bóng rơi bên trái), bóng rơi ở sân phải (player 2)
        if (scoredBy == 1) {
            x = SCREEN_WIDTH / 4;  // Giữa sân bên trái
        } else if (scoredBy == 2) {
            x = SCREEN_WIDTH * 3 / 4;  // Giữa sân bên phải
        } else {
            x = SCREEN_WIDTH / 2;  // Giữa sân (mặc định)
        }
        y = 100;
        vx = 0;
        vy = 0;
        active = true;
        lastHitBy = 0;
        touchCount = 0;
        lastTouchedTeam = 0;
    }

    void update(float groundY) {
        if (!active) return;

        x += vx;
        y += vy;
        vy += GRAVITY;

        // Va chạm với tường
        if (x - radius < 0 || x + radius > SCREEN_WIDTH) {
            vx = -vx * BALL_BOUNCE;
            if (x - radius < 0) x = radius;
            if (x + radius > SCREEN_WIDTH) x = SCREEN_WIDTH - radius;
        }

        // Va chạm với mặt đất
        if (y + radius >= groundY) {
            active = false;
        }
    }

    bool checkCollision(Player& player, bool isPlayer1, int /* netX */) {
        float closestX = std::max(player.x, std::min(x, player.x + player.width));
        float closestY = std::max(player.y, std::min(y, player.y + player.height));

        float distanceX = x - closestX;
        float distanceY = y - closestY;
        float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

        if (distanceSquared < (radius * radius)) {
            int currentTeam = isPlayer1 ? 1 : 2;

            // Nếu đội khác chạm bóng, reset số lần chạm
            if (lastTouchedTeam != currentTeam) {
                touchCount = 0;
                lastTouchedTeam = currentTeam;
            }

            touchCount++;

            // Tính toán hướng đẩy bóng
            float dx = x - (player.x + player.width / 2);
            float dy = y - (player.y + player.height / 2);
            float distance = sqrt(dx * dx + dy * dy);

            if (distance > 0) {
                dx /= distance;
                dy /= distance;

                float force = 8.0f;
                vx = dx * force;
                vy = dy * force - 2.0f;

                // Di chuyển bóng ra khỏi người chơi
                x = closestX + dx * radius;
                y = closestY + dy * radius;

                lastHitBy = currentTeam;
                return true;
            }
        }
        return false;
    }

    // Kiểm tra bóng có qua lưới không
    bool checkCrossedNet(int netX, int& previousSide) {
        int currentSide = (x < netX) ? 1 : 2;
        bool crossed = (previousSide != 0 && previousSide != currentSide);
        if (crossed) {
            // Reset số lần chạm khi bóng qua lưới
            touchCount = 0;
            lastTouchedTeam = 0;
        }
        previousSide = currentSide;
        return crossed;
    }
};

// Class Game chính
class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    bool gameOver;
    int winner;

    Player player1;
    Player player2;
    Ball ball;

    // Âm thanh
    Mix_Chunk* hitBallSound;
    Mix_Chunk* fallBallSound;
    Mix_Chunk* getPointSound;
    Mix_Music* backgroundMusic;

    float groundY;
    int netX;

    // Countdown
    int countdown;
    Uint32 countdownStartTime;
    bool isCountingDown;

    // Pause
    bool isPaused;

    // Volleyball rules
    int ballSide; // 1: left, 2: right, 0: not tracked

public:
    Game() : window(nullptr), renderer(nullptr), running(true), gameOver(false), winner(0),
             player1(100, 400), player2(650, 400),
             hitBallSound(nullptr), fallBallSound(nullptr),
             getPointSound(nullptr), backgroundMusic(nullptr),
             groundY(550), netX(SCREEN_WIDTH / 2),
             countdown(3), countdownStartTime(0), isCountingDown(true), isPaused(false),
             ballSide(0) {}

    bool init() {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
            std::cerr << "SDL không thể khởi tạo! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        window = SDL_CreateWindow("Beach Volleyball",
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (!window) {
            std::cerr << "Không thể tạo window! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Không thể tạo renderer! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Khởi tạo SDL_mixer
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            std::cerr << "SDL_mixer không thể khởi tạo! SDL_mixer Error: " << Mix_GetError() << std::endl;
            // Không return false, game vẫn chạy được không có âm thanh
        } else {
            // Load âm thanh
            hitBallSound = Mix_LoadWAV("assets/sounds/hitball.wav");
            fallBallSound = Mix_LoadWAV("assets/sounds/fallball.wav");
            getPointSound = Mix_LoadWAV("assets/sounds/getpoint.wav");
            backgroundMusic = Mix_LoadMUS("assets/sounds/background.mp3");

            if (backgroundMusic) {
                Mix_PlayMusic(backgroundMusic, -1);
                Mix_VolumeMusic(32); // Giảm âm lượng nhạc nền
            }
        }

        return true;
    }

    void handleEvents() {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }

            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_SPACE) {
                    if (gameOver) {
                        // Reset game khi game over
                        player1.score = 0;
                        player2.score = 0;
                        gameOver = false;
                        ball.reset();
                        startCountdown();
                    } else if (!isCountingDown) {
                        // Pause/Unpause khi đang chơi
                        isPaused = !isPaused;
                    }
                }
            }
        }

        if (gameOver || isCountingDown || isPaused) return;

        const Uint8* keystate = SDL_GetKeyboardState(NULL);

        // Player 1 (A/D = di chuyển, W = nhảy)
        player1.vx = 0;
        if (keystate[SDL_SCANCODE_A]) player1.vx = -PLAYER_SPEED;
        if (keystate[SDL_SCANCODE_D]) player1.vx = PLAYER_SPEED;
        if (keystate[SDL_SCANCODE_W]) player1.jump();

        // Giới hạn player 1 ở bên trái sân
        if (player1.x + player1.width > netX) player1.x = netX - player1.width;

        // Player 2 (Left/Right = di chuyển, Up = nhảy)
        player2.vx = 0;
        if (keystate[SDL_SCANCODE_LEFT]) player2.vx = -PLAYER_SPEED;
        if (keystate[SDL_SCANCODE_RIGHT]) player2.vx = PLAYER_SPEED;
        if (keystate[SDL_SCANCODE_UP]) player2.jump();

        // Giới hạn player 2 ở bên phải sân
        if (player2.x < netX) player2.x = netX;
    }

    void resetPlayersPosition() {
        player1.resetPosition();
        player2.resetPosition();
    }

    void startCountdown() {
        countdown = 3;
        countdownStartTime = SDL_GetTicks();
        isCountingDown = true;
        ball.active = false;  // Tạm dừng bóng
        ballSide = 0;  // Reset theo dõi bên sân
    }

    void updateCountdown() {
        if (!isCountingDown) return;

        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsed = currentTime - countdownStartTime;

        if (elapsed >= 1000) {
            countdown--;
            countdownStartTime = currentTime;

            if (countdown <= 0) {
                isCountingDown = false;
                ball.active = true;  // Bắt đầu chơi
            }
        }
    }

    void update() {
        if (gameOver || isPaused) return;

        // Cập nhật countdown
        if (isCountingDown) {
            updateCountdown();
            return;
        }

        player1.update(groundY);
        player2.update(groundY);

        if (ball.active) {
            ball.update(groundY);

            // Kiểm tra bóng qua lưới
            ball.checkCrossedNet(netX, ballSide);

            // Kiểm tra vi phạm 3 chạm
            bool violation = false;
            int violationScorer = 0;

            if (ball.touchCount > 3) {
                violation = true;
                // Đội vi phạm (chạm quá 3 lần) thua điểm
                violationScorer = (ball.lastTouchedTeam == 1) ? 2 : 1;
            }

            // Kiểm tra va chạm với người chơi
            bool hit1 = ball.checkCollision(player1, true, netX);
            bool hit2 = ball.checkCollision(player2, false, netX);

            if (hit1 || hit2) {
                if (hitBallSound) Mix_PlayChannel(-1, hitBallSound, 0);

                // Kiểm tra vi phạm ngay sau khi chạm
                if (ball.touchCount > 3) {
                    violation = true;
                    violationScorer = (ball.lastTouchedTeam == 1) ? 2 : 1;
                    ball.active = false;  // Dừng bóng ngay
                }
            }

            // Xử lý vi phạm
            if (violation && ball.active == false) {
                if (fallBallSound) Mix_PlayChannel(-1, fallBallSound, 0);

                if (violationScorer == 1) {
                    player1.score++;
                } else {
                    player2.score++;
                }
                if (getPointSound) Mix_PlayChannel(-1, getPointSound, 0);

                // Kiểm tra thắng
                if (player1.score >= WINNING_SCORE) {
                    gameOver = true;
                    winner = 1;
                } else if (player2.score >= WINNING_SCORE) {
                    gameOver = true;
                    winner = 2;
                } else {
                    ball.reset(violationScorer);
                    resetPlayersPosition();
                    startCountdown();
                }
            }
            // Kiểm tra bóng chạm đất
            else if (!ball.active) {
                if (fallBallSound) Mix_PlayChannel(-1, fallBallSound, 0);

                // Tính điểm
                int scorer = 0;
                if (ball.x < netX) {
                    // Bóng rơi bên trái, player 2 ghi điểm
                    player2.score++;
                    scorer = 2;
                    if (getPointSound) Mix_PlayChannel(-1, getPointSound, 0);
                } else {
                    // Bóng rơi bên phải, player 1 ghi điểm
                    player1.score++;
                    scorer = 1;
                    if (getPointSound) Mix_PlayChannel(-1, getPointSound, 0);
                }

                // Kiểm tra thắng
                if (player1.score >= WINNING_SCORE) {
                    gameOver = true;
                    winner = 1;
                } else if (player2.score >= WINNING_SCORE) {
                    gameOver = true;
                    winner = 2;
                } else {
                    ball.reset(scorer);  // Bóng rơi ở sân của người ghi điểm
                    resetPlayersPosition();  // Reset vị trí người chơi
                    startCountdown();  // Đếm ngược trước khi tiếp tục
                }
            }
        }
    }

    void render() {
        // Vẽ bầu trời
        SDL_SetRenderDrawColor(renderer, COLOR_SKY.r, COLOR_SKY.g, COLOR_SKY.b, COLOR_SKY.a);
        SDL_Rect skyRect = {0, 0, SCREEN_WIDTH, (int)groundY};
        SDL_RenderFillRect(renderer, &skyRect);

        // Vẽ cát
        SDL_SetRenderDrawColor(renderer, COLOR_SAND.r, COLOR_SAND.g, COLOR_SAND.b, COLOR_SAND.a);
        SDL_Rect sandRect = {0, (int)groundY, SCREEN_WIDTH, SCREEN_HEIGHT - (int)groundY};
        SDL_RenderFillRect(renderer, &sandRect);

        // Vẽ lưới
        SDL_SetRenderDrawColor(renderer, COLOR_NET.r, COLOR_NET.g, COLOR_NET.b, COLOR_NET.a);
        SDL_Rect netRect = {netX - 2, (int)groundY - 100, 4, 100};
        SDL_RenderFillRect(renderer, &netRect);

        // Vẽ player 1
        SDL_SetRenderDrawColor(renderer, COLOR_PLAYER1.r, COLOR_PLAYER1.g, COLOR_PLAYER1.b, COLOR_PLAYER1.a);
        SDL_Rect p1Rect = {(int)player1.x, (int)player1.y, (int)player1.width, (int)player1.height};
        SDL_RenderFillRect(renderer, &p1Rect);

        // Vẽ player 2
        SDL_SetRenderDrawColor(renderer, COLOR_PLAYER2.r, COLOR_PLAYER2.g, COLOR_PLAYER2.b, COLOR_PLAYER2.a);
        SDL_Rect p2Rect = {(int)player2.x, (int)player2.y, (int)player2.width, (int)player2.height};
        SDL_RenderFillRect(renderer, &p2Rect);

        // Vẽ bóng
        if (ball.active) {
            SDL_SetRenderDrawColor(renderer, COLOR_BALL.r, COLOR_BALL.g, COLOR_BALL.b, COLOR_BALL.a);
            // Vẽ vòng tròn bằng cách vẽ nhiều điểm
            for (int w = 0; w < ball.radius * 2; w++) {
                for (int h = 0; h < ball.radius * 2; h++) {
                    int dx = ball.radius - w;
                    int dy = ball.radius - h;
                    if ((dx*dx + dy*dy) <= (ball.radius * ball.radius)) {
                        SDL_RenderDrawPoint(renderer, ball.x + dx, ball.y + dy);
                    }
                }
            }
        }

        // Vẽ điểm số
        drawScore();

        // Vẽ màn hình game over
        if (gameOver) {
            drawGameOver();
        }

        // Vẽ countdown
        if (isCountingDown) {
            drawCountdown();
        }

        // Vẽ pause
        if (isPaused) {
            drawPause();
        }

        SDL_RenderPresent(renderer);
    }

    void drawScore() {
        // Vẽ điểm số đơn giản bằng hình chữ nhật (thay cho font)
        std::string score1 = std::to_string(player1.score);
        std::string score2 = std::to_string(player2.score);

        // Vẽ text đơn giản (có thể cải thiện bằng SDL_ttf)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        // Score player 1 (trái)
        drawNumber(150, 30, player1.score);

        // Score player 2 (phải)
        drawNumber(650, 30, player2.score);
    }

    void drawNumber(int x, int y, int num) {
        // Vẽ số đơn giản (0-9) bằng các hình chữ nhật
        const int size = 30;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        // Đơn giản hóa: vẽ số dạng digital 7-segment
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

        // Vẽ các segment
        if (seg[0]) { SDL_Rect r = {x, y, size, 5}; SDL_RenderFillRect(renderer, &r); }
        if (seg[1]) { SDL_Rect r = {x+size-5, y, 5, size/2}; SDL_RenderFillRect(renderer, &r); }
        if (seg[2]) { SDL_Rect r = {x+size-5, y+size/2, 5, size/2}; SDL_RenderFillRect(renderer, &r); }
        if (seg[3]) { SDL_Rect r = {x, y+size-5, size, 5}; SDL_RenderFillRect(renderer, &r); }
        if (seg[4]) { SDL_Rect r = {x, y+size/2, 5, size/2}; SDL_RenderFillRect(renderer, &r); }
        if (seg[5]) { SDL_Rect r = {x, y, 5, size/2}; SDL_RenderFillRect(renderer, &r); }
        if (seg[6]) { SDL_Rect r = {x, y+size/2-2, size, 5}; SDL_RenderFillRect(renderer, &r); }
    }

    void drawCountdown() {
        // Vẽ nền mờ
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
        SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &overlay);

        // Vẽ số đếm ngược lớn
        if (countdown > 0) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            int centerX = SCREEN_WIDTH / 2 - 40;
            int centerY = SCREEN_HEIGHT / 2 - 40;
            drawBigNumber(centerX, centerY, countdown, 80);
        }
    }

    void drawBigNumber(int x, int y, int num, int size) {
        // Vẽ số lớn dạng 7-segment
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

    void drawPause() {
        // Vẽ nền mờ
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
        SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &overlay);

        // Vẽ chữ "PAUSED"
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

        // Vẽ 2 thanh dọc giống nút pause
        SDL_Rect bar1 = {SCREEN_WIDTH/2 - 30, SCREEN_HEIGHT/2 - 40, 20, 80};
        SDL_Rect bar2 = {SCREEN_WIDTH/2 + 10, SCREEN_HEIGHT/2 - 40, 20, 80};
        SDL_RenderFillRect(renderer, &bar1);
        SDL_RenderFillRect(renderer, &bar2);
    }

    void drawGameOver() {
        // Vẽ nền mờ
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &overlay);

        // Vẽ text "GAME OVER" và "PLAYER X WINS"
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_Rect winnerBox = {SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 50, 300, 100};
        SDL_RenderFillRect(renderer, &winnerBox);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect textBox = {SCREEN_WIDTH/2 - 140, SCREEN_HEIGHT/2 - 40, 280, 80};
        SDL_RenderFillRect(renderer, &textBox);
    }

    void run() {
        const int FPS = 60;
        const int frameDelay = 1000 / FPS;

        Uint32 frameStart;
        int frameTime;

        // Bắt đầu countdown khi game khởi động
        startCountdown();

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

    void clean() {
        if (hitBallSound) Mix_FreeChunk(hitBallSound);
        if (fallBallSound) Mix_FreeChunk(fallBallSound);
        if (getPointSound) Mix_FreeChunk(getPointSound);
        if (backgroundMusic) Mix_FreeMusic(backgroundMusic);

        Mix_CloseAudio();

        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);

        SDL_Quit();
    }
};

int main(int argc, char* argv[]) {
    (void)argc;  // Suppress unused parameter warning
    (void)argv;  // Suppress unused parameter warning

    Game game;

    if (!game.init()) {
        std::cerr << "Không thể khởi tạo game!" << std::endl;
        return 1;
    }

    std::cout << "=== BEACH VOLLEYBALL ===" << std::endl;
    std::cout << "Player 1 (Red): A/D để di chuyển, W để nhảy" << std::endl;
    std::cout << "Player 2 (Blue): Left/Right để di chuyển, Up để nhảy" << std::endl;
    std::cout << "Người chơi đầu tiên đạt " << WINNING_SCORE << " điểm thắng!" << std::endl;
    std::cout << "Nhấn SPACE để chơi lại sau khi game kết thúc" << std::endl;

    game.run();
    game.clean();

    return 0;
}

# Beach Volleyball Game - Refactored Version

## 📂 Cấu trúc dự án

```
gamebcbb/
├── include/              # Header files
│   ├── Player.h         # Player class
│   ├── Ball.h           # Ball class
│   └── Game.h           # Game class
├── src/                 # Source files
│   ├── main.cpp         # Entry point
│   ├── Player.cpp       # Player implementation
│   ├── Ball.cpp         # Ball implementation
│   ├── Game.cpp         # Game logic
│   └── GameRender.cpp   # Game rendering
├── assets/
│   └── sounds/          # Sound files
├── build/               # Build artifacts (generated)
├── Makefile            # Build configuration
└── beach_volleyball    # Executable (generated)
```

## 🏗️ Kiến trúc Code

### 1. **Player Class** (`include/Player.h`, `src/Player.cpp`)

Quản lý logic người chơi:
- **Attributes:** position (x,y), velocity, score, ground state
- **Methods:** update(), jump(), resetPosition()
- **Constants:** GRAVITY, PLAYER_SPEED, PLAYER_JUMP

```cpp
Player* player1 = new Player(100, 400);  // Create player at position
player1->jump();                          // Make player jump
player1->update(groundY);                 // Update physics
```

### 2. **Ball Class** (`include/Ball.h`, `src/Ball.cpp`)

Quản lý logic bóng:
- **Attributes:** position, velocity, touch count, team tracking
- **Methods:** update(), checkCollision(), checkCrossedNet()
- **Volleyball Rules:** 3-touch rule enforcement

```cpp
Ball* ball = new Ball(400, 100);
ball->checkCollision(player1, true, netX);
ball->checkTouchViolation();  // Returns true if > 3 touches
```

### 3. **Game Class** (`include/Game.h`, `src/Game.cpp`, `src/GameRender.cpp`)

Game engine chính:
- **Game Loop:** init() → run() → clean()
- **State Management:** pause, countdown, game over
- **Event Handling:** keyboard input, collision detection
- **Rendering:** Tách riêng vào GameRender.cpp

```cpp
Game game;
game.init();
game.run();   // Main game loop
game.clean();
```

## 🎯 Tính năng chính

### Volleyball Rules
- ✅ Luật 3 chạm - mỗi đội tối đa 3 lần chạm bóng
- ✅ Reset touches khi bóng qua lưới
- ✅ Vi phạm → đối phương được điểm

### Game Controls
- **Player 1:** A/D (move), W (jump)
- **Player 2:** Arrow keys
- **Space:** Pause/Resume hoặc Restart
- **Winning:** First to 15 points

### Game States
- **Countdown:** 3-2-1 trước mỗi lượt
- **Pause:** Dừng game tạm thời
- **Game Over:** Hiển thị winner

## 🔧 Build & Run

```bash
# Build game
make clean
make

# Run game
./beach_volleyball

# Or combined
make run
```

## 📝 So sánh với phiên bản cũ

### Phiên bản cũ (monolithic)
- ❌ Tất cả code trong 1 file main.cpp (691 dòng)
- ❌ Sử dụng struct thay vì class
- ❌ Khó maintain và extend
- ❌ Không có encapsulation

### Phiên bản refactored (OOP)
- ✅ Chia thành 3 class riêng biệt
- ✅ Header files (.h) và implementation files (.cpp)
- ✅ Encapsulation và data hiding
- ✅ Dễ test từng component
- ✅ Dễ mở rộng tính năng mới
- ✅ Code reusable

## 🎨 Design Patterns

### 1. **Single Responsibility Principle**
- Player class: chỉ quản lý logic người chơi
- Ball class: chỉ quản lý logic bóng
- Game class: orchestrate game flow

### 2. **Encapsulation**
```cpp
// Private data, public interface
class Player {
private:
    float x, y;  // Hidden
public:
    float getX() const { return x; }  // Controlled access
};
```

### 3. **Separation of Concerns**
- Game logic (`Game.cpp`)
- Rendering logic (`GameRender.cpp`)
- Tách biệt rõ ràng

## 📊 Metrics

| Metric | Old Version | Refactored |
|--------|-------------|------------|
| Files | 1 (main.cpp) | 8 files |
| Lines per file | 691 | ~50-200 |
| Classes | 0 (structs) | 3 |
| Maintainability | Low | High |
| Testability | Hard | Easy |

## 🚀 Extending the Game

### Thêm tính năng mới dễ dàng:

```cpp
// 1. Thêm AI cho Player
class AIPlayer : public Player {
    void autoPlay(Ball* ball);
};

// 2. Thêm power-ups
class PowerUp {
    void applyEffect(Player* player);
};

// 3. Thêm nhiều loại bóng
class SpeedBall : public Ball {
    float speedMultiplier = 1.5f;
};
```

## 📚 Learning Points

1. **OOP Principles:** Inheritance, Encapsulation, Polymorphism
2. **C++ Best Practices:** Header guards, const correctness, RAII
3. **Game Architecture:** Game loop, state management, entity systems
4. **Build Systems:** Makefile with multiple source files
5. **Code Organization:** Separating interface from implementation

## 🔄 Migration Guide

Nếu muốn chuyển từ phiên bản cũ:

1. **Backup old version:**
   ```bash
   cp src/main.cpp src/main_old.cpp
   ```

2. **Use refactored version:**
   ```bash
   make clean
   make
   ./beach_volleyball
   ```

3. **Compare:**
   - Old: 691 lines monolithic
   - New: Modular, organized, maintainable

## 📄 Files Backup

- `src/main_old.cpp` - Original monolithic version
- `Makefile.old` - Original Makefile

## 🎓 Credits

Refactored from monolithic SDL2 game to OOP architecture.
Demonstrates software engineering principles in game development.

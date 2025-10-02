# Beach Volleyball Game 🏐

Game bóng chuyền bãi biển đơn giản được viết bằng C++ và SDL2.

## Tính năng

- 2 người chơi trên cùng 1 máy
- Vật lý bóng thực tế với trọng lực và độ nảy
- Hệ thống tính điểm (người chơi đầu tiên đạt 5 điểm thắng)
- Hỗ trợ âm thanh:
  - `hitball.wav` - Âm thanh khi đánh bóng
  - `fallball.wav` - Âm thanh khi bóng chạm đất
  - `getpoint.wav` - Âm thanh khi ghi điểm
  - `background.mp3` - Nhạc nền

## Yêu cầu hệ thống

- C++ compiler (g++ hoặc clang)
- SDL2
- SDL2_mixer

## Cài đặt

### macOS

```bash
brew install sdl2 sdl2_mixer
```

### Ubuntu/Debian

```bash
sudo apt-get install libsdl2-dev libsdl2-mixer-dev
```

### Arch Linux

```bash
sudo pacman -S sdl2 sdl2_mixer
```

### Windows

1. Tải SDL2 development libraries từ [libsdl.org](https://www.libsdl.org/)
2. Tải SDL2_mixer từ [SDL_mixer page](https://www.libsdl.org/projects/SDL_mixer/)
3. Giải nén và cấu hình đường dẫn trong Makefile

## Build và chạy game

```bash
# Biên dịch game
make

# Chạy game
make run

# Hoặc chạy trực tiếp
./beach_volleyball

# Xóa các file build
make clean
```

## Cách chơi

### Player 1 (Màu đỏ - Bên trái)
- **A** - Di chuyển sang trái
- **D** - Di chuyển sang phải
- **W** - Nhảy

### Player 2 (Màu xanh - Bên phải)
- **←** - Di chuyển sang trái
- **→** - Di chuyển sang phải
- **↑** - Nhảy

### Luật chơi

1. Mỗi người chơi chỉ được di chuyển trong nửa sân của mình
2. Đánh bóng qua lưới để làm bóng rơi xuống sân đối phương
3. Khi bóng chạm đất ở sân của đối phương, bạn được 1 điểm
4. Người chơi đầu tiên đạt 5 điểm thắng
5. Nhấn **SPACE** để chơi lại sau khi game kết thúc

## Thêm âm thanh

Đặt các file âm thanh vào thư mục `assets/sounds/`:

```
assets/
└── sounds/
    ├── hitball.wav      # Âm thanh đánh bóng
    ├── fallball.wav     # Âm thanh bóng rơi
    ├── getpoint.wav     # Âm thanh ghi điểm
    └── background.mp3   # Nhạc nền
```

**Lưu ý:** Game vẫn chạy được nếu không có file âm thanh, chỉ là sẽ không có tiếng.

## Tìm âm thanh miễn phí

Bạn có thể tìm âm thanh miễn phí tại:
- [Freesound.org](https://freesound.org/)
- [OpenGameArt.org](https://opengameart.org/)
- [Zapsplat.com](https://www.zapsplat.com/)

## Cấu trúc project

```
gamebcbb/
├── src/
│   └── main.cpp        # File code chính
├── assets/
│   └── sounds/         # Thư mục chứa âm thanh
├── build/              # Thư mục build (tự động tạo)
├── Makefile            # File build
└── README.md           # File hướng dẫn
```

## Tùy chỉnh

Bạn có thể chỉnh sửa các hằng số trong [src/main.cpp](src/main.cpp) để thay đổi gameplay:

```cpp
const float GRAVITY = 0.5f;           // Trọng lực
const float PLAYER_SPEED = 5.0f;       // Tốc độ di chuyển
const float PLAYER_JUMP = -12.0f;      // Lực nhảy
const float BALL_BOUNCE = 0.8f;        // Độ nảy của bóng
const int WINNING_SCORE = 5;           // Số điểm để thắng
```

## Khắc phục sự cố

### "SDL2/SDL.h: No such file or directory"
- Đảm bảo đã cài đặt SDL2 và SDL2_mixer
- Kiểm tra đường dẫn include trong Makefile

### Game không có tiếng
- Kiểm tra các file âm thanh trong `assets/sounds/`
- Đảm bảo tên file đúng và định dạng hỗ trợ (WAV, MP3)

### Lỗi linking trên macOS
```bash
# Nếu cài SDL2 qua Homebrew
export LIBRARY_PATH=/opt/homebrew/lib:$LIBRARY_PATH
export CPATH=/opt/homebrew/include:$CPATH
```

## License

MIT License - Tự do sử dụng và chỉnh sửa.

## Tác giả

Phát triển với SDL2 và C++

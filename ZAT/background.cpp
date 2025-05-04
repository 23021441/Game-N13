#include "background.h"
#include "render.h"

// Thêm các define mới
#define STAR_COUNT 200
#define PARTICLE_COUNT 50


Star stars[STAR_COUNT];
Particle particles[PARTICLE_COUNT];
bool backgroundInitialized = false;

void InitializeBackground() {
    // Khởi tạo các ngôi sao
    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i] = {
            static_cast<float>(rand() % SCREEN_WIDTH),
            static_cast<float>(rand() % SCREEN_HEIGHT),
            static_cast<float>(1 + rand() % 5) / 10.0f,
            static_cast<Uint8>(100 + rand() % 156)
        };
    }

    // Khởi tạo các hạt particle
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i] = {
            static_cast<float>(rand() % SCREEN_WIDTH),
            static_cast<float>(rand() % SCREEN_HEIGHT),
            static_cast<float>((rand() % 100) - 50) / 100.0f,
            static_cast<float>((rand() % 100) - 50) / 100.0f,
            static_cast<Uint8>(2 + rand() % 5),
            {
                static_cast<Uint8>(50 + rand() % 206),
                static_cast<Uint8>(50 + rand() % 206),
                static_cast<Uint8>(150 + rand() % 106),
                255
            }
        };
    }
    backgroundInitialized = true;
}

void UpdateBackground() {
    static Uint32 lastTime = 0;
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastTime < 16) return; // ~60fps
    lastTime = currentTime;

    // Cập nhật vị trí các ngôi sao
    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i].y += stars[i].speed;
        if (stars[i].y > SCREEN_HEIGHT) {
            stars[i].y = 0;
            stars[i].x = rand() % SCREEN_WIDTH;
        }
    }

    // Cập nhật vị trí các hạt particle
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i].x += particles[i].dx;
        particles[i].y += particles[i].dy;

        if (particles[i].x < 0 || particles[i].x > SCREEN_WIDTH)
            particles[i].dx *= -1;
        if (particles[i].y < 0 || particles[i].y > SCREEN_HEIGHT)
            particles[i].dy *= -1;
    }
}

void DrawGameBackground() {
    if (!backgroundInitialized) {
        InitializeBackground();
    }
    UpdateBackground();

    // 1. Vẽ nền đen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // 2. Vẽ các ngôi sao (hiệu ứng parallax)
    for (int i = 0; i < STAR_COUNT; i++) {
        Uint8 brightness = stars[i].alpha;
        SDL_SetRenderDrawColor(renderer, brightness, brightness, brightness, 255);
        SDL_RenderPoint(renderer, stars[i].x, stars[i].y);
    }

    // 3. Vẽ các hạt particle ánh sáng
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        SDL_SetRenderDrawColor(renderer,
                             particles[i].color.r,
                             particles[i].color.g,
                             particles[i].color.b,
                             150);
        for (int s = 0; s < particles[i].size; s++) {
            SDL_RenderPoint(renderer, particles[i].x + s, particles[i].y);
            SDL_RenderPoint(renderer, particles[i].x, particles[i].y + s);
        }
    }

    // 4. Hiệu ứng phát sáng quanh bàn cờ (giữ nguyên)
    SDL_FRect boardArea = {
        static_cast<float>(BOARD_OFFSET_X - 20),
        static_cast<float>(BOARD_OFFSET_Y - 20),
        static_cast<float>(CELL_SIZE * BOARD_SIZE + 40),
        static_cast<float>(CELL_SIZE * BOARD_SIZE + 40)
    };

    for (int i = 0; i < 10; i++) {
        SDL_SetRenderDrawColor(renderer, 0, 50, 100, 10);
        SDL_FRect glowRect = {
            boardArea.x - i,
            boardArea.y - i,
            boardArea.w + i * 2,
            boardArea.h + i * 2
        };
        SDL_RenderRect(renderer, &glowRect);
    }
}

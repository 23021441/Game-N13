#include "render.h"
#include "menu.h"
#include <cmath>
#include <SDL3/SDL_surface.h>
using namespace std;

extern SDL_Renderer* renderer;
SDL_FRect replayButtonRect;
SDL_FRect menuButtonRect;


void DrawGrid() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Vẽ đường viền xung quanh bàn cờ
    SDL_FRect border = {
        static_cast<float>(BOARD_OFFSET_X - 2),
        static_cast<float>(BOARD_OFFSET_Y - 2),
        static_cast<float>(CELL_SIZE * BOARD_SIZE + 4),
        static_cast<float>(CELL_SIZE * BOARD_SIZE + 4)
    };
    SDL_RenderRect(renderer, &border);

    // Vẽ các đường kẻ bên trong
    for (int i = 1; i < BOARD_SIZE; ++i) {
        // Đường kẻ dọc
        SDL_RenderLine(renderer,
                      static_cast<float>(BOARD_OFFSET_X + i * CELL_SIZE), static_cast<float>(BOARD_OFFSET_Y),
                      static_cast<float>(BOARD_OFFSET_X + i * CELL_SIZE), static_cast<float>(BOARD_OFFSET_Y + CELL_SIZE * BOARD_SIZE));
        // Đường kẻ ngang
        SDL_RenderLine(renderer,
                      static_cast<float>(BOARD_OFFSET_X), static_cast<float>(BOARD_OFFSET_Y + i * CELL_SIZE),
                      static_cast<float>(BOARD_OFFSET_X + CELL_SIZE * BOARD_SIZE), static_cast<float>(BOARD_OFFSET_Y + i * CELL_SIZE));
    }
}


void DrawX(int row, int col) {
    int centerX = BOARD_OFFSET_X + col * CELL_SIZE + CELL_SIZE / 2;
    int centerY = BOARD_OFFSET_Y + row * CELL_SIZE + CELL_SIZE / 2;
    int size = CELL_SIZE / 3; // Kích thước chữ X

    SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
    // Vẽ 2 đường chéo tạo thành chữ X
    SDL_RenderLine(renderer, centerX - size, centerY - size, centerX + size, centerY + size);
    SDL_RenderLine(renderer, centerX - size, centerY + size, centerX + size, centerY - size);
}

void DrawO(int row, int col) {
    int centerX = BOARD_OFFSET_X + col * CELL_SIZE + CELL_SIZE / 2;
    int centerY = BOARD_OFFSET_Y + row * CELL_SIZE + CELL_SIZE / 2;
    int radius = CELL_SIZE / 3; // Bán kính chữ O

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    // Vẽ đường tròn
    for (double angle = 0; angle < 360; angle += 1.0) {
        double rad = angle * M_PI / 180.0;
        int x1 = centerX + radius * cos(rad);
        int y1 = centerY + radius * sin(rad);
        int x2 = centerX + (radius-1) * cos(rad);
        int y2 = centerY + (radius-1) * sin(rad);
        SDL_RenderLine(renderer, x1, y1, x2, y2);
    }
}
void DrawText(const string& text, int x, int y, SDL_Color color) {
    if (!font) return;

    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(),0, color);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_FRect dest = {
                static_cast<float>(x),
                static_cast<float>(y),
                static_cast<float>(surface->w),
                static_cast<float>(surface->h)
            };
            SDL_RenderTexture(renderer, texture, NULL, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_DestroySurface(surface);
    }
}

void RenderGame() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 32, 255);
    DrawGameBackground();
    DrawTimers();
    DrawGrid();

    // Sửa vòng lặp từ 3x3 thành 6x6
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            if (board[i][j] == 1) DrawX(i, j);
            else if (board[i][j] == 2) DrawO(i, j);
        }
    }

    if (gameResult > 0) {
SDL_Color popupBg = {20, 24, 48, 240};          // Xanh đen nền
    SDL_Color buttonBg = {35, 40, 65, 255};         // Nút thường
    SDL_Color buttonHover = {60, 90, 140, 255};     // Hover sáng
    SDL_Color textColor = {255, 255, 255, 255};     // Chữ trắng
    SDL_Color borderColor = {0, 200, 255, 255};     // Neon xanh
    SDL_Color goldColor = {255, 215, 0, 255};       // Tiêu đề vàng

    const float popupWidth = 400;
    const float popupHeight = 280;
    const float buttonWidth = 220;
    const float buttonHeight = 50;
    const float buttonMargin = 20;

    SDL_FRect popupRect = {
        (SCREEN_WIDTH - popupWidth) / 2,
        (SCREEN_HEIGHT - popupHeight) / 2,
        popupWidth,
        popupHeight
    };

    // Hiệu ứng viền neon nhiều lớp
    for (int i = 3; i >= 1; --i) {
        SDL_FRect glow = {
            popupRect.x - i,
            popupRect.y - i,
            popupRect.w + 2 * i,
            popupRect.h + 2 * i
        };
        SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, 60 - i * 15);
        SDL_RenderRect(renderer, &glow);
    }

    // Bóng đổ popup
    SDL_FRect shadow = popupRect;
    shadow.x += 4;
    shadow.y += 6;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
    SDL_RenderFillRect(renderer, &shadow);

    // Nền popup
    SDL_SetRenderDrawColor(renderer, popupBg.r, popupBg.g, popupBg.b, popupBg.a);
    SDL_RenderFillRect(renderer, &popupRect);

    // Viền chính
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, 200);
    SDL_RenderRect(renderer, &popupRect);

    // Tiêu đề thắng
    std::string message;
    if (gameResult == 1) message = "Người chơi 1 thắng!";
    else if (gameResult == 2) message = (currentMode == VS_COMPUTER) ? "Máy thắng!" : "Người chơi 2 thắng!";
    else message = "Hòa!";

    int textWidth = message.length() * 14;
    DrawText(message,
             popupRect.x + (popupWidth - textWidth) / 2,
             popupRect.y + 35,
             goldColor);

    // Tọa độ chuột
    float mouseX, mouseY;
    SDL_GetMouseState((float*)&mouseX, (float*)&mouseY);

    // Nút "Chơi lại"
        replayButtonRect = {
        popupRect.x + (popupWidth - buttonWidth) / 2,
        popupRect.y + 110,
        buttonWidth,
        buttonHeight
    };
    bool replayHover = (mouseX >= replayButtonRect.x && mouseX <= replayButtonRect.x + replayButtonRect.w &&
                        mouseY >= replayButtonRect.y && mouseY <= replayButtonRect.y + replayButtonRect.h);

    // Vẽ nút
    SDL_SetRenderDrawColor(renderer,
        replayHover ? buttonHover.r : buttonBg.r,
        replayHover ? buttonHover.g : buttonBg.g,
        replayHover ? buttonHover.b : buttonBg.b,
        255);
    SDL_RenderFillRect(renderer, &replayButtonRect);

    // Nút "Về menu"
    menuButtonRect = {
        replayButtonRect.x,
        replayButtonRect.y + buttonHeight + buttonMargin,
        buttonWidth,
        buttonHeight
    };
    bool menuHover = (mouseX >= menuButtonRect.x && mouseX <= menuButtonRect.x + menuButtonRect.w &&
                      mouseY >= menuButtonRect.y && mouseY <= menuButtonRect.y + menuButtonRect.h);

    SDL_SetRenderDrawColor(renderer,
        menuHover ? buttonHover.r : buttonBg.r,
        menuHover ? buttonHover.g : buttonBg.g,
        menuHover ? buttonHover.b : buttonBg.b,
        255);
    SDL_RenderFillRect(renderer, &menuButtonRect);

    // Viền khi hover
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, 255);
    if (replayHover) SDL_RenderRect(renderer, &replayButtonRect);
    if (menuHover) SDL_RenderRect(renderer, &menuButtonRect);

    // Vẽ chữ
    textWidth = 8 * 18;
    DrawText("Chơi lại",
             replayButtonRect.x + (buttonWidth - textWidth) / 2,
             replayButtonRect.y + 12,
             textColor);

    DrawText("Về menu",
             menuButtonRect.x + (buttonWidth - textWidth) / 2,
             menuButtonRect.y + 12,
             textColor);
}

SDL_RenderPresent(renderer);
}
void DrawTimers() {
    // Màu sắc
    SDL_Color activeColor = {100, 255, 255, 255};    // Xanh ngọc
    SDL_Color inactiveColor = {100, 100, 100, 255};  // Xám
    SDL_Color bgColor = {20, 20, 40, 200};           // Nền tối

    // Vẽ khung chính
    SDL_FRect timerPanel = {
        20.0f,
        20.0f,
        220.0f,
        120.0f
    };

    // Hiệu ứng viền neon
    SDL_SetRenderDrawColor(renderer, 0, 150, 255, 50);
    for (int i = 0; i < 5; i++) {
        SDL_FRect glowBorder = {
            timerPanel.x - i,
            timerPanel.y - i,
            timerPanel.w + i * 2,
            timerPanel.h + i * 2
        };
        SDL_RenderRect(renderer, &glowBorder);
    }

    // Nền khung
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(renderer, &timerPanel);

    // Vẽ thời gian người chơi 1
    SDL_Color p1Color = (currentPlayer == 1 && gameResult == 0) ? activeColor : inactiveColor;
    string p1Text = "P1: " + to_string((int)player1Time) + "s";

    // Thanh thời gian dạng neon
    SDL_FRect p1Bar = {
        40.0f,
        70.0f,
        180.0f * (player1Time / PLAYER_TIME_LIMIT),
        8.0f
    };

    // Hiệu ứng glow cho thanh
    SDL_SetRenderDrawColor(renderer, 0, 200, 255, 150);
    SDL_RenderFillRect(renderer, &p1Bar);

    // Vẽ thời gian người chơi 2/AI
    SDL_Color p2Color = (currentPlayer == 2 && gameResult == 0) ? activeColor : inactiveColor;
    string p2Text = (currentMode == VS_COMPUTER) ? "AI: " + to_string((int)player2Time) + "s"
                                                : "P2: " + to_string((int)player2Time) + "s";

    SDL_FRect p2Bar = {
        40.0f,
        110.0f,
        180.0f * (player2Time / PLAYER_TIME_LIMIT),
        8.0f
    };

    SDL_SetRenderDrawColor(renderer, 255, 100, 100, 150);
    SDL_RenderFillRect(renderer, &p2Bar);

    // Vẽ text
    DrawText(p1Text, 40, 45, p1Color);
    DrawText(p2Text, 40, 85, p2Color);


}

void DrawCosmicMenuBackground() {
    // Gradient không gian (đen - xanh tím - đen)
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        float t = static_cast<float>(y) / SCREEN_HEIGHT;
        Uint8 r = static_cast<Uint8>(10 + 20 * t);
        Uint8 g = static_cast<Uint8>(0 + 20 * (1 - t));
        Uint8 b = static_cast<Uint8>(30 + 80 * t);
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderLine(renderer, 0, y, SCREEN_WIDTH, y);
    }

    // Sao chuyển động lấp lánh
    srand(SDL_GetTicks() / 100); // đổi mỗi frame
    for (int i = 0; i < 100; i++) {
        int x = rand() % SCREEN_WIDTH;
        int y = rand() % SCREEN_HEIGHT;
        Uint8 brightness = rand() % 155 + 100;
        SDL_SetRenderDrawColor(renderer, brightness, brightness, brightness, 255);
        SDL_RenderPoint(renderer, x, y);
    }

    // Vòng xoáy thiên hà
    float centerX = SCREEN_WIDTH / 2;
    float centerY = SCREEN_HEIGHT / 2;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 30);
    for (float angle = 0; angle < 6.28f * 4; angle += 0.1f) {
        float radius = 10.0f + angle * 8.0f;
        float x = static_cast<float>(centerX + radius * cos(angle + SDL_GetTicks() * 0.001f));
        float y = static_cast<float>(centerY + radius * sin(angle + SDL_GetTicks() * 0.001f));
        SDL_RenderPoint(renderer, x, y);
    }

    // Logo Game kiểu sci-fi
    SDL_Color sciFi = {0, 255, 255, 255}; // Xanh ánh sáng laser
    DrawText("Cờ Caro", centerX - 60, 80, sciFi);


    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 80); // mờ mờ kiểu hologram


}


#include "game.h"
#include "ai.h"
#include "audio.h"
#include <iostream>
#include <SDL3/SDL_timer.h>
// Thêm vào đầu game.cpp
float player1Time = PLAYER_TIME_LIMIT;// Thời gian còn lại của người chơi 1 (khởi tạo = giới hạn thời gian)
float player2Time = PLAYER_TIME_LIMIT;// Thời gian còn lại của người chơi 2/AO (khởi tạo = giới hạn thời gian)
Uint64 lastTickTime = 0; // Thời điểm tick cuối cùng (dùng để tính delta time)
bool isPlayer1Turn = true; // Flag xác định lượt hiện tại có phải của người chơi 1 không


SDL_Window* window = nullptr; // Con trỏ cửa sổ game
SDL_Renderer* renderer = nullptr; // Con trỏ renderer để vẽ đồ họa
TTF_Font* font = nullptr;// Con trỏ font chữ dùng trong game

GameState currentState = MENU;  // Trạng thái hiện tại (MENU, SINGLE_PLAYER, MULTI_PLAYER)
GameMode currentMode = VS_COMPUTER;// Chế độ chơi (VS_COMPUTER hoặc VS_PLAYER)
int board[6][6] = {0};
int currentPlayer = 1;    // Người chơi hiện tại (1 hoặc 2)
int gameResult = 0;  // Kết quả game (0=đang chơi, 1=P1 thắng, 2=P2/AI thắng, 3=hòa)

void ResetGame() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = 0;
        }  //Xóa sạch bàn cờ 6x6 bằng cách gán tất cả ô về giá trị 0
    }
    currentPlayer = 1; // Người chơi 1 (X) đi trước
    gameResult = 0; // 0 = game chưa kết thúc
    player1Time = PLAYER_TIME_LIMIT;
    player2Time = PLAYER_TIME_LIMIT;
    isPlayer1Turn = true;
    lastTickTime = SDL_GetTicks();
}
int checkWin() {
    // Kiểm tra hàng ngang - 4 ô liên tiếp
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j <= BOARD_SIZE - 4; j++) {
            if (board[i][j] != 0 &&
                board[i][j] == board[i][j+1] &&
                board[i][j] == board[i][j+2] &&
                board[i][j] == board[i][j+3]) {

                // Kiểm tra bị chặn ở hai đầu
                bool blockedLeft = (j > 0) && (board[i][j-1] != 0);
                bool blockedRight = (j+4 < BOARD_SIZE) && (board[i][j+4] != 0);

                // Chỉ thắng nếu không bị chặn cả hai đầu
                if (!blockedLeft && !blockedRight) {
                    return board[i][j];
                }
            }
        }
    }

    // Kiểm tra hàng dọc - 4 ô liên tiếp
    for (int j = 0; j < BOARD_SIZE; j++) {
        for (int i = 0; i <= BOARD_SIZE - 4; i++) {
            if (board[i][j] != 0 &&
                board[i][j] == board[i+1][j] &&
                board[i][j] == board[i+2][j] &&
                board[i][j] == board[i+3][j]) {

                bool blockedTop = (i > 0) && (board[i-1][j] != 0);
                bool blockedBottom = (i+4 < BOARD_SIZE) && (board[i+4][j] != 0);

                if (!blockedTop && !blockedBottom) {
                    return board[i][j];
                }
            }
        }
    }

    // Kiểm tra đường chéo chính (từ trái trên xuống phải dưới) - 4 ô
    for (int i = 0; i <= BOARD_SIZE - 4; i++) {
        for (int j = 0; j <= BOARD_SIZE - 4; j++) {
            if (board[i][j] != 0 &&
                board[i][j] == board[i+1][j+1] &&
                board[i][j] == board[i+2][j+2] &&
                board[i][j] == board[i+3][j+3]) {

                bool blockedTopLeft = (i > 0 && j > 0) && (board[i-1][j-1] != 0);
                bool blockedBottomRight = (i+4 < BOARD_SIZE && j+4 < BOARD_SIZE) && (board[i+4][j+4] != 0);

                if (!blockedTopLeft && !blockedBottomRight) {
                    return board[i][j];
                }
            }
        }
    }

    // Kiểm tra đường chéo phụ (từ phải trên xuống trái dưới) - 4 ô
    for (int i = 0; i <= BOARD_SIZE - 4; i++) {
        for (int j = BOARD_SIZE - 1; j >= 3; j--) {
            if (board[i][j] != 0 &&
                board[i][j] == board[i+1][j-1] &&
                board[i][j] == board[i+2][j-2] &&
                board[i][j] == board[i+3][j-3]) {

                bool blockedTopRight = (i > 0 && j+1 < BOARD_SIZE) && (board[i-1][j+1] != 0);
                bool blockedBottomLeft = (i+4 < BOARD_SIZE && j-4 >= 0) && (board[i+4][j-4] != 0);

                if (!blockedTopRight && !blockedBottomLeft) {
                    return board[i][j];
                }
            }
        }
    }

    // Kiểm tra hòa
    bool isDraw = true;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 0) {
                isDraw = false;
                break;
            }
        }
        if (!isDraw) break;
    }

    return isDraw ? 3 : 0;
}
void HandleGameClick(float x, float y) {

    if (gameResult > 0) {
        // Kiểm tra click vào nút nào trong popup kết quả
        if (x >= replayButtonRect.x && x <= replayButtonRect.x + replayButtonRect.w &&
            y >= replayButtonRect.y && y <= replayButtonRect.y + replayButtonRect.h) {
            // Click vào nút "Chơi lại"
            ResetGame();
            return;
        }
        else if (x >= menuButtonRect.x && x <= menuButtonRect.x + menuButtonRect.w &&
                y >= menuButtonRect.y && y <= menuButtonRect.y + menuButtonRect.h) {
            // Click vào nút "Về menu"
            currentState = MENU;
            return;
        }
        return;
    }

    // Phần xử lý click khi game đang chạy bình thường
    if (x < BOARD_OFFSET_X || x >= BOARD_OFFSET_X + CELL_SIZE * BOARD_SIZE ||
        y < BOARD_OFFSET_Y || y >= BOARD_OFFSET_Y + CELL_SIZE * BOARD_SIZE) {
        return;
    }

    int row = static_cast<int>(y - BOARD_OFFSET_Y) / CELL_SIZE;
    int col = static_cast<int>(x - BOARD_OFFSET_X) / CELL_SIZE;

    if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE && board[row][col] == 0) {
        board[row][col] = currentPlayer;
        isPlayer1Turn = !isPlayer1Turn;
        gameResult = checkWin();

        if (gameResult == 0) {
            currentPlayer = (currentPlayer == 1) ? 2 : 1;

            if (currentState == SINGLE_PLAYER && currentPlayer == 2) {
                computerMove();
                isPlayer1Turn = !isPlayer1Turn;
                gameResult = checkWin();
                currentPlayer = 1;
            }
        }
    }
}
void UpdateTimers() {
    if (gameResult != 0) return; // Dừng nếu game kết thúc

     Uint64 currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTickTime) / 1000.0f; // Chuyển sang giây
    lastTickTime = currentTime;

    if (isPlayer1Turn && currentPlayer == 1) {
        player1Time -= deltaTime;
        if (player1Time <= 0) {
            player1Time = 0;
            gameResult = (currentMode == VS_COMPUTER) ? 2 : 2; // Người chơi 2 thắng
        }
    }
    else if (!isPlayer1Turn && currentPlayer == 2) {
        player2Time -= deltaTime;
        if (player2Time <= 0) {
            player2Time = 0;
            gameResult = 1; // Người chơi 1 thắng
        }
    }
}

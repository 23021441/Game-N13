#include "ai.h"
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;
// Hàm đánh giá mức độ quan trọng của ô cờ
int evaluatePosition(int row, int col) {
    // Ưu tiên trung tâm và gần trung tâm
    static int positionValue[BOARD_SIZE][BOARD_SIZE] = {
        {1, 1, 2, 2, 1, 1},
        {1, 2, 3, 3, 2, 1},
        {2, 3, 4, 4, 3, 2},
        {2, 3, 4, 4, 3, 2},
        {1, 2, 3, 3, 2, 1},
        {1, 1, 2, 2, 1, 1}
    };
    return positionValue[row][col];
}

// Hàm kiểm tra chuỗi cờ tiềm năng
int checkPotential(int player, int length) {
    int opponent = 3 - player;
    int count = 0;

    // Kiểm tra theo 4 hướng
    int directions[4][2] = {{0,1}, {1,0}, {1,1}, {1,-1}};

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] != 0) continue;

            for (int d = 0; d < 4; d++) {
                int dx = directions[d][0];
                int dy = directions[d][1];

                // Kiểm tra không vượt biên
                if (i + (length-1)*dx >= BOARD_SIZE || i + (length-1)*dx < 0 ||
                    j + (length-1)*dy >= BOARD_SIZE || j + (length-1)*dy < 0) continue;

                int playerCount = 0;
                int emptyCount = 0;
                bool valid = true;

                // Kiểm tra length ô liên tiếp
                for (int k = 0; k < length; k++) {
                    int cell = board[i + k*dx][j + k*dy];
                    if (cell == opponent) {
                        valid = false;
                        break;
                    }
                    if (cell == player) playerCount++;
                    else emptyCount++;
                }

                if (valid && playerCount == length-1 && emptyCount == 1) {
                    count++;
                }
            }
        }
    }
    return count;
}

void computerMove() {
    // 1. Thử tạo nước thắng ngay lập tức
    if (makeWinningMove(2)) return;

    // 2. Chặn nước thắng của đối thủ
    if (blockOpponentWin(2)) return;

    // 3. Tạo chuỗi 3 không bị chặn (tạo 2 đầu trống)
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 0) {
                board[i][j] = 2;
                if (checkPotential(2, 4) >= 2) { // Có thể tạo nhiều chuỗi 3
                    return;
                }
                board[i][j] = 0;
            }
        }
    }

    // 4. Chặn chuỗi 3 nguy hiểm của đối thủ
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 0) {
                board[i][j] = 1;
                if (checkPotential(1, 4) >= 2) {
                    board[i][j] = 2; // Chặn lại
                    return;
                }
                board[i][j] = 0;
            }
        }
    }

    // 5. Tạo chuỗi 2 song song (tạo đa mục tiêu)
    vector<pair<int, pair<int, int>>> moves;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 0) {
                board[i][j] = 2;
                int score = checkPotential(2, 3) * 10 + evaluatePosition(i, j);
                moves.push_back({score, {i, j}});
                board[i][j] = 0;
            }
        }
    }

    // Chọn nước đi có điểm số cao nhất
    if (!moves.empty()) {
        sort(moves.begin(), moves.end(), greater<>());
        board[moves[0].second.first][moves[0].second.second] = 2;
        return;
    }

    // 6. Nếu không có nước đi tốt, chọn ô có giá trị cao nhất
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 0) {
                board[i][j] = 2;
                return;
            }
        }
    }
}
}

bool makeWinningMove(int player) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 0) {
                board[i][j] = player;
                if (checkWin() == player) {
                    return true;
                }
                board[i][j] = 0;
            }
        }
    }
    return false;
}

bool blockOpponentWin(int player) {
    int opponent = 3 - player;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 0) {
                board[i][j] = opponent;
                if (checkWin() == opponent) {
                    board[i][j] = player;
                    return true;
                }
                board[i][j] = 0;
            }
        }
    }
    return false;
}

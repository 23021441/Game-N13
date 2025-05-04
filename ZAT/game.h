#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_ttf.h>
#include <string>

//hang so
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 1024;
const int BOARD_SIZE = 6;
const int CELL_SIZE = 80;
const int BOARD_OFFSET_X = (SCREEN_WIDTH - CELL_SIZE * BOARD_SIZE) / 2;
const int BOARD_OFFSET_Y = (SCREEN_HEIGHT - CELL_SIZE * BOARD_SIZE) / 2;
const float PLAYER_TIME_LIMIT = 180.0f;

// enum
enum GameState { MENU, SINGLE_PLAYER, MULTI_PLAYER, GAME_OVER };
enum GameMode { VS_COMPUTER, VS_PLAYER };

// cua so game
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;
extern GameState currentState;
extern GameMode currentMode;
extern int board[6][6];
extern int currentPlayer;
extern int gameResult;

// he thong thoi gian
extern float player1Time;
extern float player2Time;
extern Uint64 lastTickTime;
extern bool isPlayer1Turn;

//cac ham
void ResetGame();
int checkWin();
void HandleGameClick(float x, float y);
void UpdateTimers();
//khung bao quanh cac nut choi lai , ve menu
extern SDL_FRect replayButtonRect;
extern SDL_FRect menuButtonRect;
#endif

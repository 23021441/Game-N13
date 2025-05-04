#ifndef RENDER_H
#define RENDER_H

#include "game.h"
#include "background.h"

using namespace std;

void DrawTimers();
void DrawGrid();
void DrawX(int row, int col);
void DrawO(int row, int col);
void DrawText(const string& text, int x, int y, SDL_Color color);
void RenderGame();
void RenderMenu();


#endif

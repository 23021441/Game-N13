#ifndef MENU_H
#define MENU_H

#include "game.h"
#include <string>
#include "background.h"

using namespace std;
//cau hinh menu
extern const int MENU_ITEMS;
extern string menuText[];
extern SDL_FRect menuRects[];
extern int selectedMenuItem;

// cac ham
void RenderMenu();
void HandleMenuInput(float x, float y);
void HandleMenuClick(float x, float y);

#endif

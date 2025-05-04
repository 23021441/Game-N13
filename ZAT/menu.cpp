#include "menu.h"
#include "render.h"

using namespace std;

const int MENU_ITEMS = 3; // so muc trong menu
string menuText[MENU_ITEMS] = {"Chơi với máy", "Chơi 2 người", "Thoát game"};
SDL_FRect menuRects[MENU_ITEMS]; //vung chu nhat chua ba muc tren
int selectedMenuItem = 0;// muc dang dc chon ( muc dau tien )

void RenderMenu() {

   DrawCosmicMenuBackground();

    SDL_SetRenderDrawColor(renderer, 0, 0, 32, 255);// nen den cho background ingame

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};

    const int menuItemHeight = 60;
    const int menuSpacing = 30;
    const int totalMenuHeight = (MENU_ITEMS * menuItemHeight) + ((MENU_ITEMS - 1) * menuSpacing);
    int startY = (SCREEN_HEIGHT - totalMenuHeight) / 2;//căn giữa dọc màn hình.

    for (int i = 0; i < MENU_ITEMS; i++) {
        int textWidth = 0;
        SDL_Surface* surface = TTF_RenderText_Solid(font, menuText[i].c_str(),0, white);
        if (surface) {
            textWidth = surface->w;
            SDL_DestroySurface(surface);
        }

        int x = (SCREEN_WIDTH - textWidth) / 2;
        int y = startY + i * (menuItemHeight + menuSpacing);

        menuRects[i] = {
            static_cast<float>(x - 20),
            static_cast<float>(y - 10),
            static_cast<float>(textWidth + 40),
            static_cast<float>(menuItemHeight)
        };

        if (i == selectedMenuItem) { // ve khung bao quanh
            SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
            SDL_RenderRect(renderer, &menuRects[i]);
        }

        DrawText(menuText[i], x, y, (i == selectedMenuItem) ? yellow : white);
    }

    SDL_RenderPresent(renderer); // cap nhat frame sau khi ve
}

void HandleMenuInput(float x, float y) {
    for (int i = 0; i < MENU_ITEMS; i++) {
        if (x >= menuRects[i].x && x <= menuRects[i].x + menuRects[i].w &&
            y >= menuRects[i].y && y <= menuRects[i].y + menuRects[i].h) {
            selectedMenuItem = i;
            break;
        }
    }
}

void HandleMenuClick(float x, float y) {
    for (int i = 0; i < MENU_ITEMS; i++) {
        if (x >= menuRects[i].x && x <= menuRects[i].x + menuRects[i].w &&
            y >= menuRects[i].y && y <= menuRects[i].y + menuRects[i].h) {
            if (i == 0) {
                currentState = SINGLE_PLAYER;
                currentMode = VS_COMPUTER;
                ResetGame();
            } else if (i == 1) {
                currentState = MULTI_PLAYER;
                currentMode = VS_PLAYER;
                ResetGame();
            } else if (i == 2) {
                SDL_Event quitEvent;
                quitEvent.type = SDL_EVENT_QUIT;
                SDL_PushEvent(&quitEvent);
            }
            break;
        }
    }
}

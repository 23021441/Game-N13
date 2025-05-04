#include "game.h"
#include "render.h"
#include "menu.h"
#include "audio.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_ttf.h>
#include <SDL3/SDL_image.h>
#include <ctime>

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    srand(time(NULL));


    window = SDL_CreateWindow("Cờ Caro", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, NULL);

    font = TTF_OpenFont("arial.ttf", 36);
    if (!font) {
        SDL_Log("Không tải được font: %s", SDL_GetError());
    }

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float x, y;
                SDL_GetMouseState(&x, &y);

                if (currentState == MENU) {
                    HandleMenuClick(x, y);
                } else if (currentState == SINGLE_PLAYER || currentState == MULTI_PLAYER) {
                    HandleGameClick(x, y);
                }
            } else if (event.type == SDL_EVENT_MOUSE_MOTION && currentState == MENU) {
                float x, y;
                SDL_GetMouseState(&x, &y);
                HandleMenuInput(x, y);
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (currentState == MENU) {
                    if (event.key.key == SDLK_UP) {
                        selectedMenuItem = (selectedMenuItem - 1 + MENU_ITEMS) % MENU_ITEMS;
                    } else if (event.key.key == SDLK_DOWN) {
                        selectedMenuItem = (selectedMenuItem + 1) % MENU_ITEMS;
                    } else if (event.key.key == SDLK_RETURN) {
                        HandleMenuClick(menuRects[selectedMenuItem].x + 1,
                                      menuRects[selectedMenuItem].y + 1);
                    }
                }
            }
        }
        UpdateTimers();

        if (currentState == MENU) {
            RenderMenu();
        } else {
            RenderGame();
        }

        SDL_Delay(16);
    }

    if (font) TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

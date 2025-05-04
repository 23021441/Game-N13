// background.h
#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "game.h"

struct Star {
    float x, y;
    float speed;
    Uint8 alpha;
};

struct Particle {
    float x, y;
    float dx, dy;
    Uint8 size;
    SDL_Color color;
};

void InitializeBackground();
void UpdateBackground();
void DrawGameBackground();
void DrawCosmicMenuBackground();

#endif


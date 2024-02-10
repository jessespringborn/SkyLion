//
// Created by jesse on 2/5/24.
//

#ifndef SKYLION__SL_WINDOW_H
#define SKYLION__SL_WINDOW_H

#include <SDL3/SDL.h>

typedef struct SLWindow
{
    SDL_Window* window;
    const char* title;
    int width;
    int height;
    Uint32 flags;
} SLWindow;

SLWindow* getWindow();

void createWindow();

void destroyWindow();

#endif //SKYLION__SL_WINDOW_H

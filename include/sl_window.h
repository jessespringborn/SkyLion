//
// Created by jesse on 2/5/24.
//

#ifndef SKYLION__SL_WINDOW_H
#define SKYLION__SL_WINDOW_H

#include <SDL3/SDL.h>

typedef struct sl_window
{
    SDL_Window* window;
} sl_window;

sl_window* sl_window_create(const char* title, int width, int height);

#endif //SKYLION__SL_WINDOW_H

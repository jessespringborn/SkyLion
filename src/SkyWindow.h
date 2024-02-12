//
// Created by jesse on 2/11/24.
//

#ifndef SKYLION_SRC_SKYWINDOW_H
#define SKYLION_SRC_SKYWINDOW_H

#include <SDL3/SDL.h>

typedef struct SkyWindow
{
    SDL_Window *window;
} SkyWindow;

SkyWindow * get_window();

#endif //SKYLION_SRC_SKYWINDOW_H

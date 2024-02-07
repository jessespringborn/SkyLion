//
// Created by jesse on 2/5/24.
//

#ifndef SKYLION__SL_WINDOW_H
#define SKYLION__SL_WINDOW_H

#include <SDL3/SDL.h>

extern SDL_Window* SLWindow;


void createWindow(const char* title, int width, int height);

void destroyWindow();

#endif //SKYLION__SL_WINDOW_H

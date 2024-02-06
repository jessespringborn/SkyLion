//
// Created by jesse on 2/5/24.
//

#include "../include/sl_window.h"

#include <stdlib.h>

sl_window* sl_window_create(const char* title, int width, int height)
{
    sl_window* window = malloc(sizeof(sl_window));
    window->window = SDL_CreateWindow(title, width, height,
                                      SDL_WINDOW_VULKAN);
    return window;
}
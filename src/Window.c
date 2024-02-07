//
// Created by jesse on 2/5/24.
//

#include "SkyLion.h"

#include <stdlib.h>
#include <stdio.h>

SDL_Window* SLWindow;

void
createWindow(const char* title, int width, int height)
{
    printf("Creating window...\n");

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }

    SLWindow = SDL_CreateWindow(title, width, height,
                                      SDL_WINDOW_VULKAN);

    if (!SLWindow)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    printf("Window created.\n");
}

void
destroyWindow()
{
    printf("Destroying window...\n");
    SDL_DestroyWindow(SLWindow);
    printf("Window destroyed.\n");
    SDL_Quit();
}
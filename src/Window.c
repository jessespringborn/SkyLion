//
// Created by jesse on 2/5/24.
//

#include "SkyLion.h"

#include <stdlib.h>
#include <stdio.h>


SLWindow*
getWindow()
{
    static SLWindow window;
    static bool initialized = false;
    if (!initialized)
    {
        window.window = NULL;
        initialized = true;
        printf("Creating window...\n");

        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            printf("SDL_Init Error: %s\n", SDL_GetError());
            exit(1);
        }

        window.title = "SkyLion";
        window.width = 800;
        window.height = 600;
        window.flags = SDL_WINDOW_VULKAN;

        window.window = SDL_CreateWindow(window.title, window.width, window.height,
                                    window.flags);

        if (!window.window)
        {
            printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
            SDL_Quit();
            exit(1);
        }

        printf("Window created.\n");
    }

    return &window;
}

void
createWindow()
{
    getWindow();
}

void
destroyWindow()
{
    printf("Destroying window...\n");
    SDL_DestroyWindow(getWindow()->window);
    printf("Window destroyed.\n");
    SDL_Quit();
}
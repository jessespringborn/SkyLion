//
// Created by jesse on 2/6/24.
//

#include "skylion.h"

#include <stdio.h>
#include <stdbool.h>

#include "sl_window.h"

void CoreLoop()
{
    bool running = true;
    while (running)
    {
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }
    }
}

void
ShutDown()
{
    printf("Shutting down Sky Lion Engine...\n");

    SDL_DestroyWindow(SLWindow);
    SDL_Quit();

    printf("Sky Lion Engine shut down.\n");
}

int
Start()
{
    printf("Starting Sky Lion Engine...\n");

    CreateWindow("Sky Lion", 800, 600);
    createRenderer();
    CoreLoop();
    return 0;
}

int
main()
{
    Start();
    ShutDown();
    return 0;
}

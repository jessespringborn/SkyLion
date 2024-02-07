//
// Created by jesse on 2/6/24.
//

#include "SkyLion.h"

#include <stdio.h>
#include <stdbool.h>

#include "Window.h"

void coreLoop()
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
shutDown()
{
    printf("Shutting down Sky Lion Engine...\n");

    destroyRenderer();
    destroyWindow();

    printf("Sky Lion Engine shut down.\n");
}

int
run()
{
    printf("Starting Sky Lion Engine...\n");

    createWindow("Sky Lion", 800, 600);
    createRenderer();

    coreLoop();

    shutDown();

    return 0;
}

int
main()
{
    run();

    return 0;
}

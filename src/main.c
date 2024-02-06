#include <stdio.h>
#include <stdbool.h>

#include "sl_window.h"

int
main()
{

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    sl_window *window = sl_window_create("Hello, World!", 640, 480);
    if (window == NULL)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return 1;
    }

    // game loop
    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }
    }

    printf("Hello, World!\n");
    return 0;
}












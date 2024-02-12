//
// Created by jesse on 2/11/24.
//

#include "SkyLion.h"


SkyWindow *
get_window()
{
    static SkyWindow window;
    if (window.window == NULL)
    {
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            printf("SDL_Init Error: %s\n", SDL_GetError());
            exit(SKY_ERROR_SDL_INIT);
        }

        window.window = SDL_CreateWindow("Hello, World!", 800, 600, SDL_WINDOW_VULKAN);

        if (window.window == NULL)
        {
            printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
            exit(SKY_ERROR_WINDOW_CREATION);
        }
    }
    return &window;
}

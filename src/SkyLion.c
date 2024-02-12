//
// Created by jesse on 2/6/24.
//

#include "SkyLion.h"


void
core_loop()
{
    bool is_running = true;
    while (is_running)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    is_running = false;
                    break;
            }
        }
    }
}

int
main()
{
    get_window();
    create_renderer();

    core_loop();

    destroy_renderer();

    return SKY_SUCCESS;
}

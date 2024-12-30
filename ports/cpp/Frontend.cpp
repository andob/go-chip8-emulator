#include "Frontend.h"
#include <iostream>
#include <SDL.h>

using namespace std;

#define BLOCK_WIDTH 15
#define BLOCK_HEIGHT 15

#define HandleSDLError(condition, function) if ((condition) != 0) \
{ \
    cout << "Error in " << #function << ": " << SDL_GetError() << endl; \
    return; \
}

void Frontend::run() const
{
    constexpr u32 window_width = BLOCK_WIDTH * DISPLAY_WIDTH;
    constexpr u32 window_height = BLOCK_HEIGHT * DISPLAY_HEIGHT;

    i32 error = SDL_Init(SDL_INIT_VIDEO);
    HandleSDLError(error, SDL_Init);

    SDL_Window* window = SDL_CreateWindow(/*title*/ "CHIP-8",
        /*position*/ SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        /*size*/ window_width, window_height,
        /*flags*/ SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    HandleSDLError(!window, SDL_CreateWindow);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    HandleSDLError(!renderer, SDL_CreateRenderer);

    error = SDL_InitSubSystem(SDL_INIT_EVENTS);
    HandleSDLError(error, SDL_InitSubSystem);

    while (true)
    {
        this->render_frame(renderer);

        const EventHandlerResult result = this->handle_events();
        if (result.should_exit_frontend)
        {
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return;
        }

        this->system->vblank();
    }
}

void Frontend::render_frame(SDL_Renderer* renderer) const
{
    i32 error = SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    HandleSDLError(error, SDL_SetRenderDrawColor);

    error = SDL_RenderClear(renderer);
    HandleSDLError(error, SDL_RenderClear);

    this->system->iterate_display([renderer](const u8 x, const u8 y, const bool pixel)
    {
        i32 error = SDL_SetRenderDrawColor(renderer, 0x00, pixel ? 0xFF : 0x00, 0x00, 0xFF);
        HandleSDLError(error, SDL_SetRenderDrawColor);

        SDL_Rect rectangle;
        rectangle.x = x * BLOCK_WIDTH;
        rectangle.y = y * BLOCK_HEIGHT;
        rectangle.w = BLOCK_WIDTH;
        rectangle.h = BLOCK_HEIGHT;
        error = SDL_RenderFillRect(renderer, &rectangle);
        HandleSDLError(error, SDL_RenderFillRect);
    });

    SDL_RenderPresent(renderer);
    SDL_Delay(17);
}

Frontend::EventHandlerResult Frontend::handle_events() const
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if ((event.type == SDL_QUIT) || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
        {
            return EventHandlerResult { .should_exit_frontend = true };
        }

        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        {
            u8 key = 0x00;
            switch (event.key.keysym.sym)
            {
                case SDLK_KP_1: key = 0x01; break;
                case SDLK_KP_2: case SDLK_UP: key = 0x02; break;
                case SDLK_KP_3: key = 0x03; break;
                case SDLK_KP_4: key = 0x0C; break;
                case SDLK_q: case SDLK_LEFT: key = 0x04; break;
                case SDLK_w: case SDLK_SPACE: key = 0x05; break;
                case SDLK_e: case SDLK_RIGHT: key = 0x06; break;
                case SDLK_r: key = 0x0D; break;
                case SDLK_a: key = 0x07; break;
                case SDLK_s: case SDLK_DOWN: key = 0x08; break;
                case SDLK_d: key = 0x09; break;
                case SDLK_f: key = 0x0E; break;
                case SDLK_z: key = 0x0A; break;
                case SDLK_x: key = 0x00; break;
                case SDLK_c: key = 0x0B; break;
                case SDLK_v: key = 0x0F; break;
                default: continue;
            }

            if (event.type == SDL_KEYDOWN)
            {
                this->system->notify_on_key_down(key);
            }
            else if (event.type == SDL_KEYUP)
            {
                this->system->notify_on_key_up(key);
            }
        }
    }

    return EventHandlerResult { .should_exit_frontend = false };
}

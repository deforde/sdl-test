#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#ifdef _WIN32
#undef main
#endif

#include "data/data.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

typedef struct {
    SDL_Window* window;
    SDL_Surface* window_surface;
    SDL_Surface* spaceship_surface;
} game_state_t;

game_state_t state;

void init_game_state()
{
    state.window = NULL;
    state.window_surface = NULL;
    state.spaceship_surface = NULL;
}

void destroy_game_state()
{
    SDL_FreeSurface(state.spaceship_surface);
    SDL_FreeSurface(state.window_surface);
    if(state.window == NULL) {
        SDL_DestroyWindow(state.window);
    }
}

void init()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    init_game_state();

    state.window = SDL_CreateWindow(
        "sdl-test",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if(state.window == NULL) {
        fprintf(stderr, "SDL window could not be created: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    state.window_surface = SDL_GetWindowSurface(state.window);
    SDL_FillRect(state.window_surface, NULL, SDL_MapRGB(state.window_surface->format, 0x0, 0x0, 0x0));

    SDL_RWops* rw = SDL_RWFromConstMem(spaceship_bitmap, sizeof(spaceship_bitmap));
    state.spaceship_surface = SDL_LoadBMP_RW(rw, 1);
}

void destroy()
{
    destroy_game_state();
}

void render()
{
    SDL_Rect dest_coords = {
        .x = SCREEN_WIDTH / 2,
        .y = SCREEN_HEIGHT / 2
    };
    SDL_BlitSurface(state.spaceship_surface, NULL, state.window_surface, &dest_coords);

    SDL_UpdateWindowSurface(state.window);
}

int main(int argc, char* argv[])
{
    init();

    for(;;) {
        // Poll for events
        bool running = true;
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
            }
        }
        if(!running)
            break;

        // Updated game state

        // Render
        render();

        SDL_Delay(1000 / 60);
    }

    destroy();

    SDL_Quit();

    return EXIT_SUCCESS;
}

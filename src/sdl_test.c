#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#ifdef _WIN32
#undef main
#endif

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(int argc, char* argv[])
{
    if(SDL_Init( SDL_INIT_VIDEO ) < 0) {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_CreateWindow(
        "sdl-test",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    return EXIT_SUCCESS;
}

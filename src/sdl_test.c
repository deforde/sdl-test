#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#ifdef _WIN32
#undef main
#endif

// ============================================================================
// Global definitions
// ============================================================================

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

const char* spaceship_img = "data/ship.png";

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* spaceship_texture;
} game_state_t;

game_state_t state;

// ============================================================================
// Forward declarations
// ============================================================================

void init();
void destroy();
void render();

// ============================================================================
// Function implementations
// ============================================================================

void init()
{
    state.window = NULL;
    state.renderer = NULL;
    state.spaceship_texture = NULL;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    state.window = SDL_CreateWindow(
        "sdl-test",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if(state.window == NULL) {
        fprintf(stderr, "SDL window could not be created: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_ACCELERATED);
    if(state.renderer == NULL) {
        fprintf(stderr, "SDL renderer could not be created: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_SetRenderDrawColor(state.renderer, 0x0, 0x0, 0x0, 0xFF);

    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
        fprintf(stderr, "SDL image could no be initialised: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Surface* spaceship_surface = IMG_Load(spaceship_img);
    if(spaceship_surface == NULL) {
        fprintf(stderr, "Failed to load image from: \"%s\": %s\n", spaceship_img, IMG_GetError());
        exit(EXIT_FAILURE);
    }

    state.spaceship_texture = SDL_CreateTextureFromSurface(state.renderer, spaceship_surface);
    if(state.spaceship_texture == NULL) {
        fprintf(stderr, "SDL texture could not be created: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_FreeSurface(spaceship_surface);
}

void destroy()
{
    SDL_DestroyTexture(state.spaceship_texture);
    state.spaceship_texture = NULL;

    SDL_DestroyRenderer(state.renderer);
    state.renderer = NULL;

    SDL_DestroyWindow(state.window);
    state.window = NULL;

    IMG_Quit();
    SDL_Quit();
}

void render()
{
    SDL_RenderClear(state.renderer);

    SDL_RenderCopy(state.renderer, state.spaceship_texture, NULL, NULL);

    SDL_RenderPresent(state.renderer);
}

// ============================================================================
// Main entry point
// ============================================================================

int main(int argc, char* argv[])
{
    init();

    bool running = true;
    SDL_Event event;
    while(running) {
        // Poll for events
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Updated game state

        // Render
        render();

        SDL_Delay(1000 / 60);
    }

    destroy();

    return EXIT_SUCCESS;
}

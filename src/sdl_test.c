#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
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

enum {
    SPACESHIP_STATIONARY_1,
    SPACESHIP_STATIONARY_2,
    SPACESHIP_BANK_LEFT_1,
    SPACESHIP_BANK_LEFT_2,
    SPACESHIP_BANK_HARD_LEFT_1,
    SPACESHIP_BANK_HARD_LEFT_2,
    SPACESHIP_BANK_RIGHT_1,
    SPACESHIP_BANK_RIGHT_2,
    SPACESHIP_BANK_HARD_RIGHT_1,
    SPACESHIP_BANK_HARD_RIGHT_2,
    SPACESHIP_SPRITES_TOTAL
};

SDL_Rect spaceship_sprite_quads[SPACESHIP_SPRITES_TOTAL];

typedef struct {
    int32_t x;
    int32_t y;
} vector_t;

typedef struct {
    vector_t position;
    vector_t velocity;

    SDL_Texture* texture;

    SDL_Rect* sprite_quad;
    uint32_t sprite_scaling;
    SDL_Rect render_quad;

    int32_t num_animation_frames;
    int32_t animation_idx;
    int32_t num_rendered_frames_per_animation_frame;
    int32_t rendered_frame_idx;
} spaceship_t;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    spaceship_t spaceship;
} game_state_t;

game_state_t state;

// ============================================================================
// Forward declarations
// ============================================================================

void init();
void destroy();
void update_state();
void render();

// ============================================================================
// Function implementations
// ============================================================================

void init()
{
    spaceship_sprite_quads[SPACESHIP_BANK_HARD_LEFT_1].x = 0;
    spaceship_sprite_quads[SPACESHIP_BANK_HARD_LEFT_1].y = 0;
    spaceship_sprite_quads[SPACESHIP_BANK_HARD_LEFT_1].w = 16;
    spaceship_sprite_quads[SPACESHIP_BANK_HARD_LEFT_1].h = 24;

    spaceship_sprite_quads[SPACESHIP_BANK_LEFT_1].x = 16;
    spaceship_sprite_quads[SPACESHIP_BANK_LEFT_1].y = 0;
    spaceship_sprite_quads[SPACESHIP_BANK_LEFT_1].w = 16;
    spaceship_sprite_quads[SPACESHIP_BANK_LEFT_1].h = 24;

    spaceship_sprite_quads[SPACESHIP_STATIONARY_1].x = 32;
    spaceship_sprite_quads[SPACESHIP_STATIONARY_1].y = 0;
    spaceship_sprite_quads[SPACESHIP_STATIONARY_1].w = 16;
    spaceship_sprite_quads[SPACESHIP_STATIONARY_1].h = 24;

    spaceship_sprite_quads[SPACESHIP_BANK_RIGHT_1].x = 48;
    spaceship_sprite_quads[SPACESHIP_BANK_RIGHT_1].y = 0;
    spaceship_sprite_quads[SPACESHIP_BANK_RIGHT_1].w = 16;
    spaceship_sprite_quads[SPACESHIP_BANK_RIGHT_1].h = 24;

    spaceship_sprite_quads[SPACESHIP_BANK_HARD_RIGHT_1].x = 64;
    spaceship_sprite_quads[SPACESHIP_BANK_HARD_RIGHT_1].y = 0;
    spaceship_sprite_quads[SPACESHIP_BANK_HARD_RIGHT_1].w = 16;
    spaceship_sprite_quads[SPACESHIP_BANK_HARD_RIGHT_1].h = 24;

    spaceship_sprite_quads[SPACESHIP_BANK_HARD_LEFT_2].x = 0;
    spaceship_sprite_quads[SPACESHIP_BANK_HARD_LEFT_2].y = 24;
    spaceship_sprite_quads[SPACESHIP_BANK_HARD_LEFT_2].w = 16;
    spaceship_sprite_quads[SPACESHIP_BANK_HARD_LEFT_2].h = 24;

    spaceship_sprite_quads[SPACESHIP_BANK_LEFT_2].x = 16;
    spaceship_sprite_quads[SPACESHIP_BANK_LEFT_2].y = 24;
    spaceship_sprite_quads[SPACESHIP_BANK_LEFT_2].w = 16;
    spaceship_sprite_quads[SPACESHIP_BANK_LEFT_2].h = 24;

    spaceship_sprite_quads[SPACESHIP_STATIONARY_2].x = 32;
    spaceship_sprite_quads[SPACESHIP_STATIONARY_2].y = 24;
    spaceship_sprite_quads[SPACESHIP_STATIONARY_2].w = 16;
    spaceship_sprite_quads[SPACESHIP_STATIONARY_2].h = 24;

    spaceship_sprite_quads[SPACESHIP_BANK_RIGHT_2].x = 48;
    spaceship_sprite_quads[SPACESHIP_BANK_RIGHT_2].y = 24;
    spaceship_sprite_quads[SPACESHIP_BANK_RIGHT_2].w = 16;
    spaceship_sprite_quads[SPACESHIP_BANK_RIGHT_2].h = 24;

    spaceship_sprite_quads[SPACESHIP_BANK_HARD_RIGHT_2].x = 64;
    spaceship_sprite_quads[SPACESHIP_BANK_HARD_RIGHT_2].y = 24;
    spaceship_sprite_quads[SPACESHIP_BANK_HARD_RIGHT_2].w = 16;
    spaceship_sprite_quads[SPACESHIP_BANK_HARD_RIGHT_2].h = 24;

    state.window = NULL;
    state.renderer = NULL;

    state.spaceship.position.x = SCREEN_WIDTH / 2;
    state.spaceship.position.y = SCREEN_HEIGHT / 2;
    state.spaceship.velocity.x = 0;
    state.spaceship.velocity.y = 0;
    state.spaceship.texture = NULL;
    state.spaceship.sprite_quad = NULL;
    state.spaceship.sprite_scaling = 4;
    state.spaceship.render_quad.x = 0;
    state.spaceship.render_quad.y = 0;
    state.spaceship.render_quad.w = 0;
    state.spaceship.render_quad.h = 0;
    state.spaceship.num_animation_frames = 2;
    state.spaceship.animation_idx = 0;
    state.spaceship.num_rendered_frames_per_animation_frame = 4;
    state.spaceship.rendered_frame_idx = 0;

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

    state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(state.renderer == NULL) {
        fprintf(stderr, "SDL renderer could not be created: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

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

    state.spaceship.texture = SDL_CreateTextureFromSurface(state.renderer, spaceship_surface);
    if(state.spaceship.texture == NULL) {
        fprintf(stderr, "SDL texture could not be created: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_FreeSurface(spaceship_surface);
}

void destroy()
{
    SDL_DestroyTexture(state.spaceship.texture);
    state.spaceship.texture = NULL;

    SDL_DestroyRenderer(state.renderer);
    state.renderer = NULL;

    SDL_DestroyWindow(state.window);
    state.window = NULL;

    IMG_Quit();
    SDL_Quit();
}

void update_state()
{
    state.spaceship.sprite_quad = &spaceship_sprite_quads[SPACESHIP_STATIONARY_1 + state.spaceship.animation_idx];
    if(state.spaceship.velocity.x < 0) {
        state.spaceship.sprite_quad = &spaceship_sprite_quads[SPACESHIP_BANK_HARD_LEFT_1 + state.spaceship.animation_idx];
    }
    else if(state.spaceship.velocity.x > 0) {
        state.spaceship.sprite_quad = &spaceship_sprite_quads[SPACESHIP_BANK_HARD_RIGHT_1 + state.spaceship.animation_idx];
    }

    state.spaceship.render_quad.x = state.spaceship.position.x - state.spaceship.sprite_quad->w * state.spaceship.sprite_scaling / 2;
    state.spaceship.render_quad.y = state.spaceship.position.y - state.spaceship.sprite_quad->h * state.spaceship.sprite_scaling / 2;
    state.spaceship.render_quad.w = state.spaceship.sprite_quad->w * state.spaceship.sprite_scaling;
    state.spaceship.render_quad.h = state.spaceship.sprite_quad->h * state.spaceship.sprite_scaling;

    ++state.spaceship.rendered_frame_idx;
    if(state.spaceship.rendered_frame_idx == state.spaceship.num_rendered_frames_per_animation_frame) {
        state.spaceship.rendered_frame_idx = 0;
        ++state.spaceship.animation_idx;
        state.spaceship.animation_idx %= state.spaceship.num_animation_frames;
    }
}

void render()
{
    SDL_SetRenderDrawColor(state.renderer, 0x0, 0x0, 0x0, 0xFF);
    SDL_RenderClear(state.renderer);

    SDL_RenderCopy(state.renderer, state.spaceship.texture, state.spaceship.sprite_quad, &state.spaceship.render_quad);

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
        update_state();

        // Render
        render();

        SDL_Delay(1000 / 60);
    }

    destroy();

    return EXIT_SUCCESS;
}

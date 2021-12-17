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

#define SPACESHIP_VELOCITY_PPS 320

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
void handle_event(const SDL_Event* const event);
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
    state.spaceship.sprite_scaling = 2;
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

void handle_event(const SDL_Event* const event)
{
    // Set the spaceship's velocity
    if(event->type == SDL_KEYDOWN && event->key.repeat == 0) {
        switch(event->key.keysym.sym) {
            case SDLK_UP: state.spaceship.velocity.y -= SPACESHIP_VELOCITY_PPS; break;
            case SDLK_DOWN: state.spaceship.velocity.y += SPACESHIP_VELOCITY_PPS; break;
            case SDLK_LEFT: state.spaceship.velocity.x -= SPACESHIP_VELOCITY_PPS; break;
            case SDLK_RIGHT: state.spaceship.velocity.x += SPACESHIP_VELOCITY_PPS; break;
        }
    }
    else if(event->type == SDL_KEYUP && event->key.repeat == 0) {
        switch(event->key.keysym.sym) {
            case SDLK_UP: state.spaceship.velocity.y += SPACESHIP_VELOCITY_PPS; break;
            case SDLK_DOWN: state.spaceship.velocity.y -= SPACESHIP_VELOCITY_PPS; break;
            case SDLK_LEFT: state.spaceship.velocity.x += SPACESHIP_VELOCITY_PPS; break;
            case SDLK_RIGHT: state.spaceship.velocity.x -= SPACESHIP_VELOCITY_PPS; break;
        }
    }
}

void update_state()
{
    // Get the time delta since the last update
    const uint32_t current_time_ms = SDL_GetTicks();
    static bool time_initialised = false;
    static uint32_t last_update_time_ms = 0;
    if(!time_initialised) {
        last_update_time_ms = current_time_ms;
        time_initialised = true;
    }
    const float time_delta_s = (current_time_ms - last_update_time_ms) / 1000.0F;
    last_update_time_ms = current_time_ms;

    // Select the correct spaceship sprite and set the rendering quad dimensions accordingly
    state.spaceship.sprite_quad = &spaceship_sprite_quads[SPACESHIP_STATIONARY_1 + state.spaceship.animation_idx];
    if(state.spaceship.velocity.x < 0) {
        state.spaceship.sprite_quad = &spaceship_sprite_quads[SPACESHIP_BANK_HARD_LEFT_1 + state.spaceship.animation_idx];
    }
    else if(state.spaceship.velocity.x > 0) {
        state.spaceship.sprite_quad = &spaceship_sprite_quads[SPACESHIP_BANK_HARD_RIGHT_1 + state.spaceship.animation_idx];
    }
    state.spaceship.render_quad.w = state.spaceship.sprite_quad->w * state.spaceship.sprite_scaling;
    state.spaceship.render_quad.h = state.spaceship.sprite_quad->h * state.spaceship.sprite_scaling;

    // Update the spaceship's position
    state.spaceship.position.x += (int32_t)(state.spaceship.velocity.x * time_delta_s);
    state.spaceship.position.y += (int32_t)(state.spaceship.velocity.y * time_delta_s);
    const int32_t spaceship_min_x = (state.spaceship.render_quad.w / 2);
    const int32_t spaceship_max_x = SCREEN_WIDTH - (state.spaceship.render_quad.w / 2);
    const int32_t spaceship_min_y = (state.spaceship.render_quad.h / 2);
    const int32_t spaceship_max_y = SCREEN_HEIGHT - (state.spaceship.render_quad.h / 2);
    state.spaceship.position.x = state.spaceship.position.x < spaceship_min_x ? spaceship_min_x : state.spaceship.position.x;
    state.spaceship.position.x = state.spaceship.position.x >= spaceship_max_x ? (spaceship_max_x - 1) : state.spaceship.position.x;
    state.spaceship.position.y = state.spaceship.position.y < spaceship_min_y ? spaceship_min_y : state.spaceship.position.y;
    state.spaceship.position.y = state.spaceship.position.y >= spaceship_max_y ? (spaceship_max_y - 1) : state.spaceship.position.y;

    // Update the spaceship's rendering quad origin
    state.spaceship.render_quad.x = state.spaceship.position.x - state.spaceship.render_quad.w / 2;
    state.spaceship.render_quad.y = state.spaceship.position.y - state.spaceship.render_quad.h / 2;

    // Update the spaceship's animation indices as needed
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
            else {
                handle_event(&event);
            }
        }

        // Updated game state
        update_state();

        // Render
        render();
    }

    destroy();

    return EXIT_SUCCESS;
}

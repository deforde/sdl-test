#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// ============================================================================
// Global definitions
// ============================================================================

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800

#define SPACESHIP_VELOCITY_PPS 320
#define SPACESHIP_FIRERATE_PPS 3

#define MAX_NUM_PROJECTILES 1024
#define PROJECTILE_VELOCITY_PPS 640

#define MAX_NUM_ENEMIES 1024
#define ENEMY_VELOCITY_PPS 160
#define ENEMY_SPAWN_RATE_EPS 1

#define MAX_NUM_EXPLOSIONS 1024

const char* spaceship_img = "../data/ship.png";
const char* projectile_img = "../data/laser-bolts.png";
const char* small_enemy_img = "../data/enemy-small.png";
const char* explosion_img = "../data/explosion.png";
const char* background_img = "../data/desert-background-looped.png";

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

enum {
    PROJECTILE_1,
    PROJECTILE_2,
    PROJECTILE_SPRITES_TOTAL
};

enum {
    SMALL_ENEMY_1,
    SMALL_ENEMY_2,
    SMALL_ENEMY_SPRITES_TOTAL
};

enum {
    EXPLOSION_1,
    EXPLOSION_2,
    EXPLOSION_3,
    EXPLOSION_4,
    EXPLOSION_5,
    EXPLOSION_TOTAL
};

SDL_Rect spaceship_sprite_quads[SPACESHIP_SPRITES_TOTAL];
SDL_Rect projectile_sprite_quads[PROJECTILE_SPRITES_TOTAL];
SDL_Rect small_enemy_sprite_quads[SMALL_ENEMY_SPRITES_TOTAL];
SDL_Rect explosion_sprite_quads[EXPLOSION_TOTAL];
SDL_Rect background_sprite_quad;

typedef struct {
    int32_t x;
    int32_t y;
} vector_t;

#define ENTITY_STRUCT_BODY                              \
{                                                       \
    vector_t position;                                  \
    vector_t velocity;                                  \
    SDL_Rect* sprite_quad;                              \
    uint32_t sprite_scaling;                            \
    SDL_Rect render_quad;                               \
    int32_t num_animation_frames;                       \
    int32_t animation_idx;                              \
    int32_t num_rendered_frames_per_animation_frame;    \
    int32_t rendered_frame_idx;                         \
}

typedef struct {
    struct ENTITY_STRUCT_BODY;
    bool is_firing;
    float time_till_next_shot_s;
} spaceship_t;

typedef struct ENTITY_STRUCT_BODY projectile_t;
typedef struct ENTITY_STRUCT_BODY enemy_t;
typedef struct ENTITY_STRUCT_BODY explosion_t;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool game_over;

    SDL_Texture* background_texture;

    SDL_Texture* spaceship_texture;
    spaceship_t spaceship;

    SDL_Texture* projectile_texture;
    projectile_t projectiles[MAX_NUM_PROJECTILES];
    size_t projectile_count;

    SDL_Texture* small_enemy_texture;
    enemy_t enemies[MAX_NUM_ENEMIES];
    size_t enemy_count;
    float time_till_next_enemy_spawn_s;

    SDL_Texture* explosion_texture;
    explosion_t explosions[MAX_NUM_EXPLOSIONS];
    size_t explosions_count;
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

SDL_Texture* load_texture(const char* const filename);

void update_background();
void update_entity_positions(float time_delta_s);
void spawn_entities(float time_delta_s);
void update_entity_animations();
void check_collisions();

void spawn_projectile();
void spawn_enemy();
void spawn_explosion(vector_t p);

bool is_collided(const SDL_Rect* const a, const SDL_Rect* const b);
bool is_contained(const vector_t* const p, const SDL_Rect* const r);

// ============================================================================
// Function implementations
// ============================================================================

void init()
{
    background_sprite_quad.x = 0;
    background_sprite_quad.y = 266;
    background_sprite_quad.w = 256;
    background_sprite_quad.h = 304;

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

    projectile_sprite_quads[PROJECTILE_1].x = 0;
    projectile_sprite_quads[PROJECTILE_1].y = 0;
    projectile_sprite_quads[PROJECTILE_1].w = 16;
    projectile_sprite_quads[PROJECTILE_1].h = 32;
    projectile_sprite_quads[PROJECTILE_2].x = 16;
    projectile_sprite_quads[PROJECTILE_2].y = 0;
    projectile_sprite_quads[PROJECTILE_2].w = 16;
    projectile_sprite_quads[PROJECTILE_2].h = 32;

    small_enemy_sprite_quads[SMALL_ENEMY_1].x = 0;
    small_enemy_sprite_quads[SMALL_ENEMY_1].y = 0;
    small_enemy_sprite_quads[SMALL_ENEMY_1].w = 16;
    small_enemy_sprite_quads[SMALL_ENEMY_1].h = 16;
    small_enemy_sprite_quads[SMALL_ENEMY_2].x = 16;
    small_enemy_sprite_quads[SMALL_ENEMY_2].y = 0;
    small_enemy_sprite_quads[SMALL_ENEMY_2].w = 16;
    small_enemy_sprite_quads[SMALL_ENEMY_2].h = 16;

    explosion_sprite_quads[EXPLOSION_1].x = 0;
    explosion_sprite_quads[EXPLOSION_1].y = 0;
    explosion_sprite_quads[EXPLOSION_1].w = 16;
    explosion_sprite_quads[EXPLOSION_1].h = 16;
    explosion_sprite_quads[EXPLOSION_2].x = 16;
    explosion_sprite_quads[EXPLOSION_2].y = 0;
    explosion_sprite_quads[EXPLOSION_2].w = 16;
    explosion_sprite_quads[EXPLOSION_2].h = 16;
    explosion_sprite_quads[EXPLOSION_3].x = 32;
    explosion_sprite_quads[EXPLOSION_3].y = 0;
    explosion_sprite_quads[EXPLOSION_3].w = 16;
    explosion_sprite_quads[EXPLOSION_3].h = 16;
    explosion_sprite_quads[EXPLOSION_4].x = 48;
    explosion_sprite_quads[EXPLOSION_4].y = 0;
    explosion_sprite_quads[EXPLOSION_4].w = 16;
    explosion_sprite_quads[EXPLOSION_4].h = 16;
    explosion_sprite_quads[EXPLOSION_5].x = 64;
    explosion_sprite_quads[EXPLOSION_5].y = 0;
    explosion_sprite_quads[EXPLOSION_5].w = 16;
    explosion_sprite_quads[EXPLOSION_5].h = 16;

    state.window = NULL;
    state.renderer = NULL;
    state.game_over = false;

    state.background_texture = NULL;
    state.spaceship_texture = NULL;
    state.projectile_texture = NULL;
    state.small_enemy_texture = NULL;
    state.explosion_texture = NULL;

    state.spaceship.sprite_scaling = 2;
    state.spaceship.position.x = SCREEN_WIDTH / 2;
    state.spaceship.position.y = SCREEN_HEIGHT - 1 - spaceship_sprite_quads[SPACESHIP_STATIONARY_1].h * state.spaceship.sprite_scaling / 2;
    state.spaceship.velocity.x = 0;
    state.spaceship.velocity.y = 0;
    state.spaceship.sprite_quad = NULL;
    state.spaceship.render_quad.x = 0;
    state.spaceship.render_quad.y = 0;
    state.spaceship.render_quad.w = 0;
    state.spaceship.render_quad.h = 0;
    state.spaceship.num_animation_frames = 2;
    state.spaceship.animation_idx = 0;
    state.spaceship.num_rendered_frames_per_animation_frame = 4;
    state.spaceship.rendered_frame_idx = 0;
    state.spaceship.is_firing = false;
    state.spaceship.time_till_next_shot_s = 0.0F;

    state.projectile_count = 0;
    state.enemy_count = 0;
    state.time_till_next_enemy_spawn_s = 0.0F;

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

    state.background_texture = load_texture(background_img);
    state.spaceship_texture = load_texture(spaceship_img);
    state.projectile_texture = load_texture(projectile_img);
    state.small_enemy_texture = load_texture(small_enemy_img);
    state.explosion_texture = load_texture(explosion_img);

    srand(time(NULL));
}

void destroy()
{
    SDL_DestroyTexture(state.explosion_texture);
    state.explosion_texture = NULL;

    SDL_DestroyTexture(state.small_enemy_texture);
    state.small_enemy_texture = NULL;

    SDL_DestroyTexture(state.projectile_texture);
    state.projectile_texture = NULL;

    SDL_DestroyTexture(state.spaceship_texture);
    state.spaceship_texture = NULL;

    SDL_DestroyTexture(state.background_texture);
    state.background_texture = NULL;

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
            case SDLK_SPACE: state.spaceship.is_firing = true; break;
        }
    }
    else if(event->type == SDL_KEYUP && event->key.repeat == 0) {
        switch(event->key.keysym.sym) {
            case SDLK_UP: state.spaceship.velocity.y += SPACESHIP_VELOCITY_PPS; break;
            case SDLK_DOWN: state.spaceship.velocity.y -= SPACESHIP_VELOCITY_PPS; break;
            case SDLK_LEFT: state.spaceship.velocity.x += SPACESHIP_VELOCITY_PPS; break;
            case SDLK_RIGHT: state.spaceship.velocity.x -= SPACESHIP_VELOCITY_PPS; break;
            case SDLK_SPACE:
                state.spaceship.is_firing = false;
                state.spaceship.time_till_next_shot_s = 0.0F;
                break;
        }
    }
}

void update_entity_positions(float time_delta_s)
{
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

    // Update all projectile's positions
    for(size_t i = 0; i < state.projectile_count; ++i) {
        projectile_t* projectile = &state.projectiles[i];
        projectile->position.y += (int32_t)(projectile->velocity.y * time_delta_s);
        projectile->render_quad.y = projectile->position.y - projectile->render_quad.h / 2;
    }
    // Remove all projectiles that have exited the screen
    for(size_t i = 0; i < state.projectile_count;) {
        if(state.projectiles[i].position.y < 0) {
            state.projectiles[i] = state.projectiles[state.projectile_count - 1];
            state.projectile_count--;
            continue;
        }
        ++i;
    }

    // Update all enemies' positions
    for(size_t i = 0; i < state.enemy_count; ++i) {
        enemy_t* enemy = &state.enemies[i];
        enemy->position.y += (int32_t)(enemy->velocity.y * time_delta_s);
        enemy->render_quad.y = enemy->position.y - enemy->render_quad.h / 2;
    }
    // Remove all enemies that have exited the screen
    for(size_t i = 0; i < state.enemy_count;) {
        if(state.enemies[i].position.y > SCREEN_HEIGHT) {
            state.enemies[i] = state.enemies[state.enemy_count - 1];
            state.enemy_count--;
            continue;
        }
        ++i;
    }
}

void spawn_entities(float time_delta_s)
{
    // If the ship is firing, and is ready to generate a new projectile, then do so now
    if(state.spaceship.is_firing) {
        if(state.spaceship.time_till_next_shot_s <= 0.0F) {
            spawn_projectile();
            state.spaceship.time_till_next_shot_s = 1.0F / SPACESHIP_FIRERATE_PPS;
        }
        else {
            state.spaceship.time_till_next_shot_s -= time_delta_s;
        }
    }

    // If enough time has elapsed, spawn an enemy
    if(state.time_till_next_enemy_spawn_s <= 0.0F) {
        spawn_enemy();
        state.time_till_next_enemy_spawn_s = 1.0F / ENEMY_SPAWN_RATE_EPS;
    }
    else {
        state.time_till_next_enemy_spawn_s -= time_delta_s;
    }
}

void update_entity_animations()
{
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
    // Update the spaceship's animation indices as needed
    ++state.spaceship.rendered_frame_idx;
    if(state.spaceship.rendered_frame_idx == state.spaceship.num_rendered_frames_per_animation_frame) {
        state.spaceship.rendered_frame_idx = 0;
        ++state.spaceship.animation_idx;
        state.spaceship.animation_idx %= state.spaceship.num_animation_frames;
    }

    // Update all projectile's animations
    for(size_t i = 0; i < state.projectile_count; ++i) {
        projectile_t* projectile = &state.projectiles[i];
        projectile->sprite_quad = &projectile_sprite_quads[PROJECTILE_1 + projectile->animation_idx];

        ++projectile->rendered_frame_idx;
        if(projectile->rendered_frame_idx == projectile->num_rendered_frames_per_animation_frame) {
            projectile->rendered_frame_idx = 0;
            ++projectile->animation_idx;
            projectile->animation_idx %= projectile->num_animation_frames;
        }
    }

    // Update all enemies' animations
    for(size_t i = 0; i < state.enemy_count; ++i) {
        enemy_t* enemy = &state.enemies[i];
        enemy->sprite_quad = &small_enemy_sprite_quads[SMALL_ENEMY_1 + enemy->animation_idx];

        ++enemy->rendered_frame_idx;
        if(enemy->rendered_frame_idx == enemy->num_rendered_frames_per_animation_frame) {
            enemy->rendered_frame_idx = 0;
            ++enemy->animation_idx;
            enemy->animation_idx %= enemy->num_animation_frames;
        }
    }

    // Update all explosions animations and remove those that have completed
    for(size_t i = 0; i < state.explosions_count;) {
        explosion_t* explosion = &state.explosions[i];
        explosion->sprite_quad = &explosion_sprite_quads[EXPLOSION_1 + explosion->animation_idx];

        ++explosion->rendered_frame_idx;
        if(explosion->rendered_frame_idx == explosion->num_rendered_frames_per_animation_frame) {
            explosion->rendered_frame_idx = 0;
            ++explosion->animation_idx;
            if(explosion->animation_idx == explosion->num_animation_frames) {
                state.explosions[i] = state.explosions[state.explosions_count - 1];
                state.explosions_count--;
                continue;
            }
        }
        ++i;
    }
}

void check_collisions()
{
    // Check enemy and projectile collisions
    for(size_t i = 0; i < state.projectile_count;) {
        projectile_t* projectile = &state.projectiles[i];
        bool collision_detected = false;

        for(size_t j = 0; j < state.enemy_count; ++j) {
            enemy_t* enemy = &state.enemies[j];

            if(is_contained(&projectile->position, &enemy->render_quad)) {
                spawn_explosion(enemy->position);
                state.enemies[j] = state.enemies[state.enemy_count - 1];
                state.enemy_count--;
                collision_detected = true;
                break;
            }
        }

        if(collision_detected) {
            state.projectiles[i] = state.projectiles[state.projectile_count - 1];
            state.projectile_count--;
            continue;
        }
        ++i;
    }

    // Check enemy and spaceship collisions
    for(size_t i = 0; i < state.enemy_count; ++i) {
        enemy_t* enemy = &state.enemies[i];
        if(is_collided(&state.spaceship.render_quad, &enemy->render_quad)) {
            spawn_explosion(state.spaceship.position);
            state.game_over = true;
            break;
        }
    }
}

void update_background()
{
    const uint32_t frames_per_scroll = 4;
    static uint32_t frame_counter = 0;

    frame_counter++;
    if(frame_counter != frames_per_scroll) {
        return;
    }
    frame_counter = 0;

    if(background_sprite_quad.y != 0) {
        background_sprite_quad.y -= 1;
    }
    else {
        background_sprite_quad.y = 304;
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

    update_entity_animations();

    if(!state.game_over) {
        update_background();

        update_entity_positions(time_delta_s);

        update_entity_animations();

        spawn_entities(time_delta_s);

        check_collisions();
    }
}

void render()
{
    SDL_SetRenderDrawColor(state.renderer, 0x0, 0x0, 0x0, 0xFF);
    SDL_RenderClear(state.renderer);

    // Render background
    SDL_Rect background_render_quad;
    background_render_quad.x = 0;
    background_render_quad.y = 0;
    background_render_quad.w = SCREEN_WIDTH;
    background_render_quad.h = SCREEN_HEIGHT;
    SDL_RenderCopy(state.renderer, state.background_texture, &background_sprite_quad, &background_render_quad);
    if(!state.game_over) {
        // Render ship
        SDL_RenderCopy(state.renderer, state.spaceship_texture, state.spaceship.sprite_quad, &state.spaceship.render_quad);
    }
    // Render projectiles
    for(size_t i = 0; i < state.projectile_count; ++i) {
        projectile_t* projectile = &state.projectiles[i];
        SDL_RenderCopy(state.renderer, state.projectile_texture, projectile->sprite_quad, &projectile->render_quad);
    }
    // Render enemies
    for(size_t i = 0; i < state.enemy_count; ++i) {
        enemy_t* enemy = &state.enemies[i];
        SDL_RenderCopy(state.renderer, state.small_enemy_texture, enemy->sprite_quad, &enemy->render_quad);
    }
    // Render explosions
    for(size_t i = 0; i < state.explosions_count; ++i) {
        explosion_t* explosion = &state.explosions[i];
        SDL_RenderCopy(state.renderer, state.explosion_texture, explosion->sprite_quad, &explosion->render_quad);
    }

    SDL_RenderPresent(state.renderer);
}

SDL_Texture* load_texture(const char* const filename)
{
    SDL_Surface* surface = IMG_Load(filename);
    if(surface == NULL) {
        fprintf(stderr, "Failed to load image from: \"%s\": %s\n", filename, IMG_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(state.renderer, surface);
    if(texture == NULL) {
        fprintf(stderr, "SDL texture could not be created: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_FreeSurface(surface);

    return texture;
}

void spawn_projectile()
{
    if(state.projectile_count < MAX_NUM_PROJECTILES) {
        projectile_t* projectile = &state.projectiles[state.projectile_count++];

        projectile->position.x = state.spaceship.position.x;
        projectile->position.y = state.spaceship.position.y + state.spaceship.render_quad.h / 2;

        projectile->velocity.x = 0;
        projectile->velocity.y = -PROJECTILE_VELOCITY_PPS;

        projectile->sprite_quad = &projectile_sprite_quads[PROJECTILE_1];
        projectile->sprite_scaling = 2;

        projectile->render_quad.w = projectile->sprite_quad->w * projectile->sprite_scaling;
        projectile->render_quad.h = projectile->sprite_quad->h * projectile->sprite_scaling;
        projectile->render_quad.x = projectile->position.x - projectile->render_quad.w / 2;
        projectile->render_quad.y = projectile->position.y - projectile->render_quad.h / 2;

        projectile->num_animation_frames = 2;
        projectile->animation_idx = 0;
        projectile->num_rendered_frames_per_animation_frame = 4;
        projectile->rendered_frame_idx = 0;
    }
}

void spawn_enemy()
{
    if(state.enemy_count < MAX_NUM_ENEMIES) {
        enemy_t* enemy = &state.enemies[state.enemy_count++];

        enemy->sprite_quad = &small_enemy_sprite_quads[SMALL_ENEMY_1];
        enemy->sprite_scaling = 2;

        enemy->render_quad.w = enemy->sprite_quad->w * enemy->sprite_scaling;
        enemy->render_quad.h = enemy->sprite_quad->h * enemy->sprite_scaling;

        const int32_t min_x = enemy->render_quad.w / 2;
        const int32_t max_x = SCREEN_WIDTH - enemy->render_quad.w / 2;

        const int32_t random_var = rand();
        const int32_t x_pos = min_x + (float)random_var / RAND_MAX * (max_x - min_x);

        enemy->position.x = x_pos;
        enemy->position.y = 0;

        enemy->velocity.x = 0;
        enemy->velocity.y = ENEMY_VELOCITY_PPS;

        enemy->render_quad.x = enemy->position.x - enemy->render_quad.w / 2;
        enemy->render_quad.y = enemy->position.y - enemy->render_quad.h / 2;

        enemy->num_animation_frames = 2;
        enemy->animation_idx = 0;
        enemy->num_rendered_frames_per_animation_frame = 4;
        enemy->rendered_frame_idx = 0;
    }
}

void spawn_explosion(vector_t p)
{
    if(state.explosions_count < MAX_NUM_EXPLOSIONS) {
        explosion_t* explosion = &state.explosions[state.explosions_count++];

        explosion->position = p;

        explosion->velocity.x = 0;
        explosion->velocity.y = 0;

        explosion->sprite_quad = &explosion_sprite_quads[EXPLOSION_1];
        explosion->sprite_scaling = 2;

        explosion->render_quad.w = explosion->sprite_quad->w * explosion->sprite_scaling;
        explosion->render_quad.h = explosion->sprite_quad->h * explosion->sprite_scaling;
        explosion->render_quad.x = explosion->position.x - explosion->render_quad.w / 2;
        explosion->render_quad.y = explosion->position.y - explosion->render_quad.h / 2;

        explosion->num_animation_frames = 2;
        explosion->animation_idx = 0;
        explosion->num_rendered_frames_per_animation_frame = 4;
        explosion->rendered_frame_idx = 0;
    }
}

bool is_collided(const SDL_Rect* const a, const SDL_Rect* const b)
{
    const vector_t a_top_l = {
        .x = a->x,
        .y = a->y
    };
    const vector_t a_bottom_l = {
        .x = a->x,
        .y = a->y + a->h
    };
    const vector_t a_top_r = {
        .x = a->x + a->w,
        .y = a->y
    };
    const vector_t a_bottom_r = {
        .x = a->x + a->w,
        .y = a->y + a->h
    };

    const bool collision_detected = is_contained(&a_top_l, b) || is_contained(&a_bottom_l, b) || is_contained(&a_top_r, b) || is_contained(&a_bottom_r, b);

    return collision_detected;
}

bool is_contained(const vector_t* const p, const SDL_Rect* const r)
{
    const bool result = p->x >= r->x && p->x <= (r->x + r->w) && p->y >= r->y && p->y <= (r->y + r->h);
    return result;
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

        // Update game state
        update_state();

        // Render
        render();
    }

    destroy();

    return EXIT_SUCCESS;
}

cmake_path(SET SDL_INCLUDE $ENV{SDL_INCLUDE})
target_include_directories(sdl_test PRIVATE ${SDL_INCLUDE})

target_sources(sdl_test
PRIVATE
    sdl_test.c
    $ENV{SDL_INCLUDE}/SDL.h
)

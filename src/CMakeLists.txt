cmake_path(SET SDL_INCLUDE $ENV{SDL_INCLUDE})
cmake_path(SET SDL_IMAGE_INCLUDE $ENV{SDL_IMAGE_INCLUDE})
target_include_directories(sdl_test PRIVATE ${SDL_INCLUDE} ${SDL_IMAGE_INCLUDE})

target_sources(sdl_test
PRIVATE
    sdl_test.c
    ${SDL_INCLUDE}/SDL.h
    ${SDL_IMAGE_INCLUDE}/SDL_image.h
)

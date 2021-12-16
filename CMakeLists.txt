cmake_minimum_required(VERSION 3.13)
project(sdl_test C)

add_executable(sdl_test "")

add_subdirectory(src)

cmake_path(SET SDL_LIB $ENV{SDL_LIB})
target_link_libraries(sdl_test ${SDL_LIB}/SDL2.lib)

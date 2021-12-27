cmake_minimum_required(VERSION 3.13)
project(sdl-test C)

add_compile_options(-Wall -Wextra -pedantic -Werror -Werror=vla)

add_executable(sdl-test "")

add_subdirectory(src)

target_link_libraries(sdl-test SDL2 SDL2_image)

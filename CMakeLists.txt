cmake_minimum_required(VERSION 3.13)
project(shmupsy C)

add_compile_options(-Wall -Wextra -pedantic -Werror -Werror=vla)

add_executable(shmupsy "")

include_directories(/usr/include/SDL2)

add_subdirectory(src)

target_link_libraries(shmupsy SDL2 SDL2_image)

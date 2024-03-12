# Minimum version
cmake_minimum_required(VERSION 3.18)

#Project info
project(
    light_sampler
    DESCRIPTION "assignment 3"
    LANGUAGES C
)

#Compiler options
set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Werror -Wpedantic -Wextra)

add_subdirectory(hal)
add_subdirectory(app)

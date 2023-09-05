cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(journalc
        VERSION 1.0
        DESCRIPTION "Not great, but learning")

set(CMAKE_C_STANDARD 11)

add_compile_options(-Wall -Wextra)

add_subdirectory(src)


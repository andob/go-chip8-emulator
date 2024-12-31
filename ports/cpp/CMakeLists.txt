cmake_minimum_required(VERSION 3.29)
project(cpp)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

add_executable(chip8 main.cpp
        constants.h
        font.h
        font.cpp
        system.h
        system.cpp
        frontend.cpp
        frontend.h
)

target_link_libraries(chip8 ${SDL2_LIBRARY})

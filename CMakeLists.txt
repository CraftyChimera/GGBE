cmake_minimum_required(VERSION 3.22)
project(DMGB CXX)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Warray-bounds")

add_executable(${PROJECT_NAME}
        main.cpp
        Base/Parser.cpp
        Cartridge/Cartridge.cpp
        Console/Console.cpp
        )

find_package(SDL2 REQUIRED)
include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})

add_subdirectory(CPU)
add_subdirectory(GPU)
add_subdirectory(Console)

target_link_libraries(${PROJECT_NAME} CPU)
target_link_libraries(${PROJECT_NAME} GPU)
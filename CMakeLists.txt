cmake_minimum_required(VERSION 3.22)
project(DMGB CXX)

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
target_link_libraries(${PROJECT_NAME} Console)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall -Wextra -pedantic -Werror)
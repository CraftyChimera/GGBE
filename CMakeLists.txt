cmake_minimum_required(VERSION 3.22)
project(GGBE CXX)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Warray-bounds -O2")

add_executable(${PROJECT_NAME} main.cpp)

find_package(SDL2 REQUIRED)
include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})

add_subdirectory(CPU)
add_subdirectory(GPU)
add_subdirectory(Base)
add_subdirectory(MMU)
add_subdirectory(APU)

target_link_libraries(${PROJECT_NAME} Base)
target_link_libraries(${PROJECT_NAME} CPU)
target_link_libraries(${PROJECT_NAME} GPU)
target_link_libraries(${PROJECT_NAME} MMU)
target_link_libraries(${PROJECT_NAME} APU)

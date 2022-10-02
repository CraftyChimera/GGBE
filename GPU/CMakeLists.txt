project(GPU)

find_package(SDL2 REQUIRED)
include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})

add_library(${PROJECT_NAME} Renderer.cpp Objects.cpp)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
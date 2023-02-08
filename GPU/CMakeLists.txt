project(GPU)

find_package(SDL2 REQUIRED)
include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})

add_library(${PROJECT_NAME} Gpu.cpp Pixel_Mapper.cpp Pixel_Mapper.hpp GPU_Utility.hpp GPU_Utility.cpp)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC "../Base" "../Console")
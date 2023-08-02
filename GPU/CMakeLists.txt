project(GPU)

find_package(SDL2 REQUIRED)
include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})

add_library(${PROJECT_NAME}
        Gpu.cpp
        PixelMapper.cpp
        GpuUtility.cpp)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC "../Base" "../Console")
project(Base)

add_library(${PROJECT_NAME} Console.cpp Boot.cpp Parser.cpp)

find_package(SDL2 REQUIRED)
include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC "../CPU" "../GPU" "../MMU")
cmake_minimum_required(VERSION 3.10)

project(Squirrel)

set(CMAKE_CXX_STANDARD 20) # need c++20 for string format
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(raylib REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)

if(APPLE)
  target_link_libraries(
    ${PROJECT_NAME}
    raylib
    "-framework CoreVideo"
    "-framework IOKit"
    "-framework Cocoa"
    "-framework GLUT"
    "-framework OpenGL")
else()
  target_link_libraries(${PROJECT_NAME} raylib)
endif()

target_include_directories(Squirrel PRIVATE include)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

cmake_minimum_required(VERSION 3.10)

project(Squirrel)

set(CMAKE_CXX_STANDARD 20) # need c++20 for string format
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find raylib config ( emscripten )
set(raylib_DIR /opt/homebrew/opt/raylib/lib/cmake/raylib)

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(Squirrel PRIVATE include)

# include and link raylib
target_include_directories(Squirrel PRIVATE 
  /Users/rafa/downloads/raylib/src
)

# WARNING: this is in ur DOWNLOADS folder!
target_link_libraries(Squirrel PRIVATE 
  /Users/rafa/downloads/raylib/src/web/libraylib.web.a
)

# set output file extension to .html
set_target_properties(Squirrel PROPERTIES SUFFIX ".html")

# set link flags
# better than using set_target_properties ( more modern, no string )
# each separate elem is its own thing so that's why flags are attached directly to arg
#       ( e.g. -s USE_GLFW... ---> -sUSE_GLFW )
#       emscripten parser supports both
target_link_options(Squirrel PRIVATE
    -Os 
    -sUSE_GLFW=3
    -sASYNCIFY
    -DPLATFORM_WEB
    --shell-file /Users/rafa/downloads/raylib/src/shell.html

  # Os -> optimize for size
  # USE_GLFW -> tell emscripten to use its own wasm glfw ( !!!!! )
  # asyncify -> allow running sync code in browser's async env ( yeah, not sure either man... )
  # dplatform_web -> tells raylib it's being compiled for web ( i assume to set some ifndefs... )
  #         ^^^ DO WE RLY NEED? we already compiled web binary?
  #                   >>> YES WE DO! sets RAYLIB headers
  # shell file: html template that emscripten wraps game in; sets up canvas, fullscreen, etc.

  # why do they need to be attached?
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

cmake_minimum_required(VERSION 3.20)

project(TownAndTavern
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Download and build raylib as part of this project.
include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "Do not build raylib examples" FORCE)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

add_executable(TownAndTavern
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
)

target_include_directories(TownAndTavern PRIVATE include)
target_link_libraries(TownAndTavern PRIVATE raylib)

# raylib may need these system libraries on Linux.
if(UNIX AND NOT APPLE)
    target_link_libraries(TownAndTavern PRIVATE m pthread dl rt X11)
endif()

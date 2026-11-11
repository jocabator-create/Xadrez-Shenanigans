cmake_minimum_required(VERSION 3.5)
project(XadrezAcademico)

set(CMAKE_CXX_STANDARD 17)

# Força a Raylib a compilar em modo PLATFORM_ANDROID e desliga tudo o resto
set(PLATFORM "PLATFORM_ANDROID" CACHE STRING "Target platform" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
set(WITH_examples  OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.zip
)
FetchContent_MakeAvailable(raylib)

add_executable(XadrezAcademico main.cpp)

target_link_libraries(XadrezAcademico PRIVATE raylib)

if(ANDROID)
    target_link_libraries(XadrezAcademico PRIVATE android EGL GLESv2 m z)
endif()

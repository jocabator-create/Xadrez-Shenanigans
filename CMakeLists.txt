cmake_minimum_required(VERSION 3.16)
project(XadrezAcademico)

set(CMAKE_CXX_STANDARD 17)

# Baixa e integra a Raylib automaticamente via FetchContent (ideal para Android e multiplataforma)
include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.zip
)
# Configurações para não compilar exemplos desnecessários da raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

add_executable(XadrezAcademico main.cpp)

target_link_libraries(XadrezAcademico PRIVATE raylib)

if(ANDROID)
    target_link_libraries(XadrezAcademico PRIVATE android EGL GLESv2 m z)
endif()

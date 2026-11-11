cmake_minimum_required(VERSION 3.16)
project(XadrezAcademico)

set(CMAKE_CXX_STANDARD 17)

# Encontra ou descarrega a Raylib compatível com Android
find_package(raylib REQUIRED)

add_executable(XadrezAcademico main.cpp)

target_link_libraries(XadrezAcademico PRIVATE raylib)

if(ANDROID)
    target_link_libraries(XadrezAcademico PRIVATE android EGL GLESv2 m z)
endif()

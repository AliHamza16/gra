cmake_minimum_required(VERSION 3.24)
project(gra
        VERSION 1.0.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

add_executable(gra src/main.cpp)

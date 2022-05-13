cmake_minimum_required(VERSION 3.18)
project(rope-cpp)

include_directories(include/)
set(CMAKE_CXX_STANDARD 20)

add_executable(rope-cpp src/main.cpp)

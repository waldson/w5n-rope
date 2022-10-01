cmake_minimum_required(VERSION 3.19)
project(rope-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include/)

option(BUILD_TESTS "Build the tests" ON)
option(BUILD_SHARED_LIBS "Build shared libs" ON)
option(UTF8_SUPPORT "Add utf-8 support with uni-algo" ON)

include(FetchContent)

if(UTF8_SUPPORT)
    FetchContent_Declare(uni-algo
    GIT_REPOSITORY https://github.com/uni-algo/uni-algo.git
    GIT_TAG v0.5.0)
    FetchContent_MakeAvailable(uni-algo)
endif()

add_library(w5n-rope STATIC src/Rope.cpp)

if (BUILD_SHARED_LIBS)
    add_library(w5n-rope-shared SHARED src/Rope.cpp)
endif()

add_executable(rope-cpp src/main.cpp src/Rope.cpp)
if(UTF8_SUPPORT)
    target_link_libraries(rope-cpp PRIVATE uni-algo::uni-algo)
    target_compile_definitions(rope-cpp PRIVATE W5N_ROPE_UTF8_SUPPORT=1)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

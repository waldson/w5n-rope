cmake_minimum_required(VERSION 3.19)
project(
  w5n-rope
  VERSION 0.1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build the tests" ON)
option(W5N_ROPE_UTF8_IGNORE "Add utf-8 support with uni-algo" OFF)

add_subdirectory(src)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

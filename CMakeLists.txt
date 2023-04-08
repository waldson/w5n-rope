cmake_minimum_required(VERSION 3.19)
project(
  w5n-rope
  VERSION 0.1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wempty-body -Werror -Werror=maybe-uninitialized -Warray-bounds -g)

option(BUILD_TESTS "Build the tests" OFF)
option(BUILD_EXAMPLES "Build the tests" OFF)
option(W5N_ROPE_UTF8_IGNORE "Remove support for utf-8 graphemes" OFF)

add_subdirectory(src)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

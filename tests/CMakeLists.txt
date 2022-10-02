cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(GoogleTest)

FetchContent_Declare(googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0
)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
# FetchContent_MakeAvailable(googletest)

# from: http://cliutils.gitlab.io/modern-cmake/chapters/testing/googletest.html
macro(package_add_test TESTNAME)
  # create an executable in which the tests will be stored
  add_executable(${TESTNAME} ${ARGN})
  # link the Google test infrastructure, mocking library, and a default main function to
  # the test executable.  Remove g_test_main if writing your own main function.

  if(UTF8_SUPPORT)
    target_link_libraries(${TESTNAME} PRIVATE uni-algo::uni-algo)
    target_compile_definitions(${TESTNAME} PRIVATE W5N_ROPE_UTF8_SUPPORT=1)
  endif()
  target_link_libraries(${TESTNAME} PRIVATE gtest gmock gtest_main w5n-rope::w5n-rope)
  # gtest_discover_tests replaces gtest_add_tests,
  # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
  gtest_discover_tests(
    ${TESTNAME}
    # set a working directory so your project root so that you can find test data via paths relative to the project root
    WORKING_DIRECTORY ${PROJECT_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
  )
  set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
  target_include_directories(${TESTNAME} PRIVATE ../include)
endmacro()

package_add_test(
  RopeTest
  src/RopeTest.cpp
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_executable(basic-example ./basic.cpp)
target_link_libraries(basic-example w5n-rope::w5n-rope)

add_executable(emoji-example ./emoji.cpp)
target_link_libraries(emoji-example w5n-rope::w5n-rope)

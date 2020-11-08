cmake_minimum_required(VERSION 3.15)
project(ConsoleEngine)

set(CMAKE_CXX_STANDARD 17)
set(LIB_HEADERS src/include/Colors.h src/include/Chars.h src/include/Rect.h src/include/Sprite.h src/include/ConsoleEngine.h)
set(LIB_SOURCE src/Sprite.cpp src/ConsoleEngine.cpp)
set(LIB_FILES ${LIB_HEADERS} ${LIB_SOURCE})

add_compile_options("/Os")
add_compile_options("/Oy")
add_compile_options("/Gy")

add_library(ConsoleEngine ${LIB_FILES})

add_executable(main src/MainTest.cpp)
target_link_libraries(main ConsoleEngine)
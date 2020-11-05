cmake_minimum_required(VERSION 3.15)
project(ConsoleEngine)

set(CMAKE_CXX_STANDARD 17)
set(LIB_HEADERS ConsoleEngine.h Colors.h Chars.h Rect.h Glyph.h)
set(LIB_SOURCE ConsoleEngine.cpp)
set(LIB_FILES ${LIB_HEADERS} ${LIB_SOURCE} )

add_compile_options("/Os")
add_compile_options("/Oy")
add_compile_options("/Gy")

add_library(ConsoleEngine ${LIB_FILES})

add_executable(main MainTest.cpp)
target_link_libraries(main ConsoleEngine)
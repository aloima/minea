cmake_minimum_required(VERSION 3.19)
project(minea VERSION 0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS yes)
set(CMAKE_BUILD_TYPE Release)

file(GLOB SOURCES "src/*.c")
add_executable(minea ${SOURCES})
target_include_directories(minea PUBLIC .)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
target_link_libraries(minea ${CURSES_LIBRARIES} menu)

install(TARGETS minea)

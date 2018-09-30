# CMakeLists.txt
#
# By Sebastian Raaphorst, 2018.

cmake_minimum_required(VERSION 3.12)
project(constexpr_sudoku)

set(CMAKE_CXX_STANDARD 17)

# We definitely want O0 here, as all the work is done in compilation.
# Thus, making the compiler work harder to improve runtime makes no sense.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_definitions(-std=c++2a -stdlib=libc++ -g -Wall -O0 -fconstexpr-steps=2147483647)
    set(CMAKE_EXE_LINKER_FLAGS "-stdlib=libc++")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_definitions(-std=c++2a -g -Wall -O0)
    set(CMAKE_EXE_LINKER_FLAGS "")
endif()

add_executable(sudoku_constexpr sudoku_constexpr.cpp)

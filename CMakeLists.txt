cmake_minimum_required(VERSION 3.10)
project(Mahjong)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-O3)

add_executable(test test.cpp types.h constants.h constants.cpp tiles.h utils.h yaku_analysis.cpp hand.cpp tiles.cpp)

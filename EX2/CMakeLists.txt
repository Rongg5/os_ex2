cmake_minimum_required(VERSION 3.24)
project(EX2)

set(CMAKE_CXX_STANDARD 17)

add_executable(EX2 uthreads.cpp Thread.h Thread.cpp)

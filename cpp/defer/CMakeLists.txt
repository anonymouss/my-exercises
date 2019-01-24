cmake_minimum_required (VERSION 3.12)

project (mydefer)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSION OFF)

add_executable (mydefer.out main.cpp Defer.hpp)

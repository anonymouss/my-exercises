cmake_minimum_required (VERSION 3.12)

project (streamparser)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSION OFF)

include_directories (include)

add_executable (streamparser.out main.cpp AvcParser.cpp)

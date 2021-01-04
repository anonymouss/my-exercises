cmake_minimum_required(VERSION 3.10)

project(simple_nn)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# add_definitions(-D_DEBUG_)

list(APPEND NN_INCLUDE nn/include)
list(APPEND NN_SOURCE nn)

include_directories(${NN_INCLUDE})

file(GLOB NN_SOURCE_FILES ${NN_SOURCE}/*.cpp)

add_executable(simpleNN.out main.cpp ${NN_SOURCE_FILES})
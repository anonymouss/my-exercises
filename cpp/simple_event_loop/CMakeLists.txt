cmake_minimum_required(VERSION 3.10)

project(simple_event_loop)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# add_definitions(-D_DEBUG_)

list(APPEND EVENT_INCLUDE event/include)
list(APPEND EVENT_SOURCE event)

include_directories(${EVENT_INCLUDE})

file(GLOB EVENT_SOURCE_FILES ${EVENT_SOURCE}/*.cpp)

add_executable(simpleEventLoop.out main.cpp ${EVENT_SOURCE_FILES})
target_link_libraries (simpleEventLoop.out pthread)
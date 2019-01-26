cmake_minimum_required (VERSION 3.12)

project (mylog)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSION OFF)

add_executable (mylog.out main.cpp MyLog.cpp)
target_link_libraries (mylog.out pthread)

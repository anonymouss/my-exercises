cmake_minimum_required(VERSION 3.12)

project(typeinfo)

# use standard C++ 17, disallow extensions, need compiler support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSION OFF)

# if compiler is Clang, link libc++, otherwise, link default libstdc++
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
endif()

add_executable (typeinfo.out main.cpp)
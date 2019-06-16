cmake_minimum_required(VERSION 3.14)
project(cpp_wheel)

set(CMAKE_CXX_STANDARD 14)

add_executable(cpp_wheel main.cpp
        noncopyable/noncopyable.h)
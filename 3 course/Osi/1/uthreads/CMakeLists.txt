cmake_minimum_required(VERSION 3.22)
project(uthreads)

set(CMAKE_CXX_STANDARD 17)

add_executable(uthreads main.cpp uthread.cpp uthread.h)

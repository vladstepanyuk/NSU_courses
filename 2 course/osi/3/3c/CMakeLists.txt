cmake_minimum_required(VERSION 3.16)
project(3c)

set(CMAKE_CXX_STANDARD 17)

add_executable(3c main.cpp FileUtils.cpp FileUtils.h)

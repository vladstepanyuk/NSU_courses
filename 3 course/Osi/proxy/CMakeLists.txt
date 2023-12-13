cmake_minimum_required(VERSION 3.22)
project(proxy)

set(CMAKE_CXX_STANDARD 17)

add_executable(proxy main.cpp
        ThreadPool.cpp
        ThreadPool.h
        Proxy.cpp
        Proxy.h)

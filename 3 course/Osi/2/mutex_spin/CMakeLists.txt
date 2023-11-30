cmake_minimum_required(VERSION 3.22)
project(mutex_spin C)

set(CMAKE_C_STANDARD 11)

add_executable(mutex_spin main.c
        sync.c
        sync.h)

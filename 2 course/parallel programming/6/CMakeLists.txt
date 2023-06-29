cmake_minimum_required(VERSION 3.22)
project(6)

set(CMAKE_CXX_STANDARD 17)

add_executable(6 main.cpp Task.cpp Task.h Mpi_manager.cpp Mpi_manager.h Tasks_list.cpp Tasks_list.h Sender.cpp Sender.h)

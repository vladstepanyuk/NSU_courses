cmake_minimum_required(VERSION 3.22)
project(5)

set(CMAKE_CXX_STANDARD 11)

add_executable(5 main.cpp Field.h Field.cpp InputFileParser.cpp InputFileParser.h LifeConway.cpp LifeConway.h MPIManager.cpp MPIManager.h)

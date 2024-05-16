cmake_minimum_required(VERSION 3.22)
project(DiscreteAlgo)
project(AntiContour)

set(CMAKE_CXX_STANDARD 20)

add_executable(DiscreteAlgo main.cpp
        graph_equal.cpp
        graph_brutforce.cpp)

add_executable(AntiContour main_calc_anticontours.cpp)

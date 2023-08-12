cmake_minimum_required(VERSION 3.22)
project(4)

set(CMAKE_CXX_STANDARD 17)

include_directories(SYSTEM ${MPI_INCLUDE_PATH})
#target_link_libraries(4 ${MPI_CXX_LIBRARIES})

add_executable(4 main.cpp Solution.cpp Solution.h  MatrixUtils.cpp MatrixUtils.h )

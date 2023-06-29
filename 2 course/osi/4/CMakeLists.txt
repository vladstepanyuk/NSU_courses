cmake_minimum_required(VERSION 3.22)
project(4)
project(4_TEST)


set(CMAKE_CXX_STANDARD 17)


add_executable(4 main.cpp a.cpp a.h b.cpp b.h Utils.cpp Utils.h c.cpp c.h)

add_executable(4_TEST a.cpp a.h b.cpp b.h Utils.cpp Utils.h c.cpp c.h testMain.cpp)


cmake_minimum_required(VERSION 3.22)
project(Server)
project(Client)

set(CMAKE_CXX_STANDARD 17)

add_executable(Server main_server.cpp Server.cpp Server.h)
add_executable(Client Client.cpp Client.h main_client.cpp)

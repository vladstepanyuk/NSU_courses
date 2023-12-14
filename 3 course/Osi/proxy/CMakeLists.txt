cmake_minimum_required(VERSION 3.22)
project(proxy)

set(CMAKE_CXX_STANDARD 17)

FIND_PACKAGE( Boost 1.75 COMPONENTS program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )


add_executable(proxy main.cpp
        Proxy.cpp
        Proxy.h)

TARGET_LINK_LIBRARIES( proxy LINK_PUBLIC ${Boost_LIBRARIES} )



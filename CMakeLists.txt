cmake_minimum_required(VERSION 3.7)
project(algorithm)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        restaurant.cpp
        restaurant.h
        district.cpp
        district.h
        dispatcher.cpp
        dispatcher.h
        order.cpp
        order.h
        point.cpp
        point.h
        proc_static.h
        main.cpp
        munkres.cpp
        munkres.h
        matrix.cpp
        matrix.h)
add_executable(algorithm ${SOURCE_FILES})
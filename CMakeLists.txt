cmake_minimum_required(VERSION 3.5)
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
        proc.h
        main.cpp)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_executable(algorithm ${SOURCE_FILES})
target_link_libraries(algorithm Threads::Threads)
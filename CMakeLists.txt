cmake_minimum_required(VERSION 3.19)
project(MySkipList)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(MySkipList
        skiplist.h
        main.cpp)

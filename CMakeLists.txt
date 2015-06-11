cmake_minimum_required(VERSION 3.2)
project(BigInt)
include_directories(${BigInt_SOURCE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -static -std=c++11")

set(SOURCE_FILES big_integer.cpp)
add_executable(BigInt ${SOURCE_FILES})
add_executable(big_integer_testing
    big_integer_testing.cpp
    big_integer.h
    big_integer.cpp
    gtest/gtest-all.cc
    gtest/gtest.h
    gtest/gtest_main.cc)
target_link_libraries(big_integer_testing -lpthread)
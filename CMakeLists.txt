cmake_minimum_required(VERSION 3.2)
project(BigInteger)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(BigInteger ${SOURCE_FILES} big_integer.cpp big_integer.h big_integer_testing.cpp)
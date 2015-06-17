cmake_minimum_required(VERSION 3.2)
project(BigInt)
include_directories(${BigInt_SOURCE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -std=c++11")

set(SOURCE_FILES big_integer.cpp)
add_executable(BigInt ${SOURCE_FILES})
add_executable(big_integer_testing
    big_integer_testing.cpp
    big_integer.h
    big_integer.cpp
    my_vector.h
    my_vector.cpp
    gtest/gtest-all.cc
    gtest/gtest.h
    gtest/gtest_main.cc)
add_executable(main
    main.cpp
    big_integer.h
    big_integer.cpp
    gtest/gtest-all.cc
    gtest/gtest.h
    gtest/gtest_main.cc)
target_link_libraries(big_integer_testing -lpthread)
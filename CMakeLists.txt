cmake_minimum_required(VERSION 2.8.4)
project(primes)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Weverything -Wno-c++98-compat -O2")

set(SOURCE_FILES main.cpp)
add_executable(primes ${SOURCE_FILES})
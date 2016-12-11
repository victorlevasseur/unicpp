cmake_minimum_required(VERSION 2.6)
project(UniCppTests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SRC_FILES String.cpp Utf8Tools.cpp tests/Tests.cpp)

add_executable(UniCpp_tests ${SRC_FILES})
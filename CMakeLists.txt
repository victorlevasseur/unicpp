cmake_minimum_required(VERSION 2.6)
project(UniCppTests)

add_subdirectory(utf8proc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SRC_FILES String.cpp Unit.cpp Utf8Tools.cpp utf8proc/utf8proc.c  tests/Tests.cpp)

add_executable(UniCpp_tests ${SRC_FILES})
target_link_libraries(UniCpp_tests utf8proc)

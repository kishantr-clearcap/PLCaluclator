cmake_minimum_required(VERSION 3.8.2)
project(TickCalculator)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Action.cpp Action.h Contract.cpp Contract.h)
add_executable(TickCalculator ${SOURCE_FILES})

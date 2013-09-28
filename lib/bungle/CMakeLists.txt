cmake_minimum_required(VERSION 2.8)

set(BUNGLE_SOURCES
    "bungle.hpp"
    "bungle.cpp")
add_library(bungle STATIC ${BUNGLE_SOURCES})
if(CMAKE_COMPILER_IS_GNUCXX)
    set_target_properties(bungle PROPERTIES COMPILE_FLAGS "-std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX)

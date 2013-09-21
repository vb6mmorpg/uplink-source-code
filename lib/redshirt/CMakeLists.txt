cmake_minimum_required(VERSION 2.8)

set(REDSHIRT_SOURCES
    "hash.h"
    "redshirt.h"
    "hash.cpp"
    "redshirt.cpp")
add_library(redshirt STATIC ${REDSHIRT_SOURCES})

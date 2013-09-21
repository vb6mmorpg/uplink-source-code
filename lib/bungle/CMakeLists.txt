cmake_minimum_required(VERSION 2.8)

set(BUNGLE_SOURCES
    "bungle.h"
    "bungle.cpp")
add_library(bungle STATIC ${BUNGLE_SOURCES})
target_link_libraries(bungle tosser mmgr)

cmake_minimum_required(VERSION 2.8)

set(MMGR_SOURCES
    "mmgr.h"
    "nommgr.h"
    "mmgr.cpp")
add_library(mmgr STATIC ${MMGR_SOURCES})

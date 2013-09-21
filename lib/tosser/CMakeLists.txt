cmake_minimum_required(VERSION 2.8)

set(TOSSER_SOURCES
    "tosser.h"
    "btree.cpp"
    "darray.cpp"
    "llist.cpp")
add_library(tosser STATIC ${TOSSER_SOURCES})

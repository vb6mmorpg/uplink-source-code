cmake_minimum_required(VERSION 2.8)

set(TOSSER_SOURCES
    "llist.cpp"
    "darray.cpp"
    "btree.cpp"
    "tosser.h")
add_library(tosser STATIC ${TOSSER_SOURCES})
target_link_libraries(tosser mmgr)

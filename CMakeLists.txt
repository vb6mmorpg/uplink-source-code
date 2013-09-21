cmake_minimum_required(VERSION 2.8)

project(uplink CXX)

include_directories(
    ${CMAKE_SOURCE_DIR}/contrib
    ${CMAKE_SOURCE_DIR}/lib/mmgr
    ${CMAKE_SOURCE_DIR}/lib/tosser
    ${CMAKE_SOURCE_DIR}/lib/bungle)

add_subdirectory(contrib)
add_subdirectory(lib)

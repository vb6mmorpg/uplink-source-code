cmake_minimum_required(VERSION 2.8)

project(uplink CXX)

set(UPLINK_CONTRIB ${CMAKE_SOURCE_DIR}/contrib)
set(UPLINK_LIB ${CMAKE_SOURCE_DIR}/lib)

include_directories(${UPLINK_CONTRIB}
    ${UPLINK_CONTRIB}/SDL2-2.0.0/include
    ${UPLINK_LIB}/mmgr
    ${UPLINK_LIB}/tosser
    ${UPLINK_LIB}/bungle)

add_subdirectory(${UPLINK_CONTRIB})
add_subdirectory(${UPLINK_LIB})

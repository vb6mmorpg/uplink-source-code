cmake_minimum_required(VERSION 2.8)

project(uplink CXX C)

set(SSE_FLAGS "-mmmx -msse -msse2")
set(CMAKE_C_FLAGS ${SSE_FLAGS})
set(CMAKE_CXX_FLAGS ${SSE_FLAGS})

set(UPLINK_CONTRIB ${CMAKE_SOURCE_DIR}/contrib)
set(UPLINK_LIB ${CMAKE_SOURCE_DIR}/lib)

include_directories(${CMAKE_BINARY_DIR}/contrib/SDL2-2.0.0/include
    ${CMAKE_BINARY_DIR}/contrib/libmikmod-3.3.1/include
    ${CMAKE_BINARY_DIR}/contrib/zlib-1.2.8
    ${UPLINK_CONTRIB}
    ${UPLINK_CONTRIB}/freetype-2.5.0.1/include
    ${UPLINK_CONTRIB}/freetype-2.5.0.1/include/freetype
    ${UPLINK_CONTRIB}/SDL2-2.0.0/include
    ${UPLINK_CONTRIB}/zlib-1.2.8
    ${UPLINK_LIB}/mmgr
    ${UPLINK_LIB}/tosser
    ${UPLINK_LIB}/bungle)

add_subdirectory(${UPLINK_CONTRIB})
add_subdirectory(${UPLINK_LIB})

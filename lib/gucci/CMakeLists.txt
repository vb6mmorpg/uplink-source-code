cmake_minimum_required(VERSION 2.8)

set(GUCCI_SOURCES
    "gucci.hpp"
    "image.hpp"
    "gucci.cpp"
    "image.cpp")
add_library(gucci STATIC ${GUCCI_SOURCES})
set_target_properties(gucci PROPERTIES COMPILE_FLAGS "-std=c++11")
target_link_libraries(gucci SDL2main SDL2 SDL2_ttf SDL2_image)

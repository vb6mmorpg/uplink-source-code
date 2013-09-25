cmake_minimum_required(VERSION 2.8)

set(GUCCI_SOURCES
    "drawable.hpp"
    "gucci.hpp"
    "image.hpp"
    "drawable.cpp"
    "gucci.cpp"
    "image.cpp")
add_library(gucci STATIC ${GUCCI_SOURCES})
if(CMAKE_COMPILER_IS_GNUCXX)
    set_target_properties(gucci PROPERTIES COMPILE_FLAGS "-std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX)
target_link_libraries(gucci SDL2main SDL2 SDL2_ttf SDL2_image)

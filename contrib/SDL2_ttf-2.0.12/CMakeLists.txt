cmake_minimum_required(VERSION 2.8)

set(SDL_TTF_SOURCES
    "SDL_ttf.h"
    "SDL_ttf.c")
if(WIN32)
    set(SDL_TTF_SOURCES ${SDL_TTF_SOURCES} "version.rc")
endif(WIN32)
add_library(SDL2_ttf SHARED ${SDL_TTF_SOURCES})
target_link_libraries(SDL2_ttf SDL2main SDL2 freetype)

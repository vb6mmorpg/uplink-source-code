set(GUCCI_SRCS
    gucci.cpp
    image.cpp
    gucci_sdl.cpp
)

set(GUCCI_HDRS
    gucci.h
    gucci_internal.h
    image.h
)

include_directories(${FTGL_INCLUDE_DIR})
include_directories(${FREETYPE_INCLUDE_DIR_freetype2})
add_definitions(-DUSE_SDL -DUSE_FTGL)

add_library(gucci STATIC ${GUCCI_SRCS} ${GUCCI_HDRS})

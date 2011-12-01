set(SOUNDGARDEN_SRCS
    soundgarden.cpp
    soundgarden_sdlmixer.cpp
    sgplaylist.cpp
)

set(SOUNDGARDEN_HDRS
    soundgarden.h
    sgplaylist.h
)

add_definitions(-DUSE_SDL)

add_library(soundgarden STATIC ${SOUNDGARDEN_SRCS} ${SOUNDGARDEN_HDRS})

set(ECLIPSE_SRCS
    animation.cpp
    button.cpp
    eclipse.cpp
)

set(ECLIPSE_HDRS
    animation.h
    button.h
    eclipse.h
)

add_library(eclipse STATIC ${ECLIPSE_SRCS} ${ECLIPSE_HDRS})
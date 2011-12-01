set(BUNGLE_SRCS
    bungle.cpp
)

set(BUNGLE_HDRS
    bungle.h
)

add_library(bungle STATIC ${BUNGLE_SRCS} ${BUNGLE_HDRS})
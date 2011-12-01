set(REDSHIRT_SRCS
    hash.cpp
    redshirt.cpp
)

set(REDSHIRT_HDRS
    hash.h
    redshirt.h
)

add_library(redshirt STATIC ${REDSHIRT_SRCS} ${REDSHIRT_HDRS})
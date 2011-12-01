set(MMGR_SRCS
    mmgr.cpp
)

set(MMGR_HDRS
    mmgr.h
    nommgr.h
)

add_library(mmgr STATIC ${MMGR_SRCS} ${MMGR_HDRS})
set(VANBAKEL_SRCS
    interface.cpp
    task.cpp
    taskwrapper.cpp
    vanbakel.cpp
)

set(VANBAKEL_HDRS
    interface.h
    task.h
    taskwrapper.h
    vanbakel.h
)

add_library(vanbakel STATIC ${VANBAKEL_SRCS} ${VANBAKEL_HDRS})
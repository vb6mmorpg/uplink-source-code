set(IRCLIB_SRCS
    irc.cpp
    socket.cpp
    CrossThreadsMessagingDevice.cpp
)

if(UNIX)
    set(IRCLIB_SRCS ${IRCLIB_SRCS}
        linux/windows.cpp
        linux/missing.cpp
        linux/winsock.cpp
    )
    include_directories(linux)
endif()

add_library(irclib STATIC ${IRCLIB_SRCS})
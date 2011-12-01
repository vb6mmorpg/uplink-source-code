set(TCP4U_SRCS
    src/http4u.c
    src/htp4_url.c
    src/htp4_err.c
    src/util.c
    src/tcp4u.c
    src/tcp4_err.c
    src/tcp4_log.c
    src/tcp4u_ex.c
    src/skt4u.c
    src/tn4u.c
    src/tn_proto.c
    src/udp4u.c
    src/smtp4u.c
)

include_directories(Include)
add_definitions(-DUNIX)

add_library(tcp4u STATIC ${TCP4U_SRCS})
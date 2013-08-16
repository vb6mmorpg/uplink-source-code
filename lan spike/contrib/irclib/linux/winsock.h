#if !defined __WINSOCK_COMPAT_H
#define __WINSOCK_COMPAT_H

#include "windows.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/* Silly file for Visual C++ */

typedef void *WSADATA;
typedef int SOCKET;

#define HOSTENT struct hostent
#define IN_ADDR in_addr

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

int WSAStartup(WORD versionRequested, WSADATA *w);
void WSACleanup();
int closesocket(SOCKET);

#endif // __WINSOCK_COMPAT_H

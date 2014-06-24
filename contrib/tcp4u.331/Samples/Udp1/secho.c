/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* an echo server                                                            */
/* This app waits incoming frame on echo port. Datagrams are returned        */
/* to their sender.                                                          */
/*                                                                           */
/* Please read file ../build.txt before compiling this sample                */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <errno.h>

#include <tcp4u.h>
#include <udp4u.h>

#undef TRUE
#undef FALSE
#define  TRUE   (1==1)
#define  FALSE  (0==1)


char buf [1024];
#define HLOG HFILE_ERROR

int main (int argc, char *argv[])
{
int Rc, NbBytes;
LPUDPSOCK lpSock=NULL;
char szVer [128];

  Tcp4uVer (szVer, sizeof szVer);
  printf ("Using %s\n", szVer);


  Tcp4uInit();
  Rc = UdpInit (& lpSock,  NULL, 0, 7);
  for ( ; Rc>=TCP4U_SUCCESS; )
  {
     Rc= UdpRecv (lpSock, buf, sizeof buf, 0, HLOG);
     NbBytes = Rc;
     /* addressee will be the sender of previous datagram */
     if (Rc>=TCP4U_SUCCESS) 
        Rc = UdpBind (lpSock, TRUE, UDP4U_SERVER);

printf ("Receive from %08X\n", * (int *) & lpSock->saRecvAddr.sin_addr);
printf ("Will send to %08X\n", * (int *) & lpSock->saSendAddr.sin_addr);
     if (Rc>=TCP4U_SUCCESS) 
        Rc = UdpSend (lpSock, buf, NbBytes, 0, HLOG);
     if (Rc>=TCP4U_SUCCESS) 
        printf ("Clients sent %d bytes. String <%s>\n", NbBytes, buf);
     /* accepts datagrams from any host */
     UdpBind (lpSock, FALSE, 0);
  }
  printf ("Error %d/%d: <%s>\n", Rc, errno, Tcp4uErrorString (Rc));
  if (lpSock!=NULL)   UdpCleanup (lpSock);
  Tcp4uCleanup();
}


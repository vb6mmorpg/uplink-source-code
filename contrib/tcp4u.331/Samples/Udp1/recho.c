/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* an echo client                                                            */
/* This app sends a frame to the given echo server and receive the answer    */
/*                                                                           */
/* Please read file ../build.txt before compiling this sample                */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <errno.h>

#include <tcp4u.h>
#include <udp4u.h>

char buf [1024];

#define HLOG -1
#ifndef TRUE
#  define TRUE (1==1)
#endif

#define STRING "Hello echo server"

void Usage (void)
{
   printf ("Usage: recho [-v] <echo server>\n");
   exit(0);
}

int main (int argc, char *argv[])
{
int Rc;
LPUDPSOCK lpSock=NULL;
char szVer [128];
int  nTries=0;
unsigned short nPort;

  if (argc<2)  Usage();
  if (argc==3  &&  strcmp (argv[1], "-v")==0)  Tcp4uEnableLog (LOG4U_DUMP);

  Tcp4uVer (szVer, sizeof szVer);   printf ("Using %s\n", szVer);
  Tcp4uInit();

  nPort = Udp4uServiceToPort ("echo");
  printf ("echo port is %u\n", nPort);

  Rc = UdpInit (& lpSock, argv[argc-1], nPort, 0);
  if (Rc>=TCP4U_SUCCESS) 
     Rc = UdpBind (lpSock, TRUE, UDP4U_CLIENT);
  
  if (Rc<TCP4U_SUCCESS) 
  {
    printf ("Error %d/%d: <%s>\n", Rc, errno, Tcp4uErrorString (Rc));
    return 0;
  }

  /* send 3 datagrams */
     nTries++;
     do
     {
        Rc = UdpSend (lpSock, STRING, sizeof STRING, 0, HLOG);
        if (Rc>=TCP4U_SUCCESS) 
           Rc= UdpRecv (lpSock, buf, sizeof buf, 5, HLOG);
        nTries++;
     }
     while (nTries <= 3 && Rc==TCP4U_TIMEOUT);

  if (Rc>=TCP4U_SUCCESS) 
       printf ("Server sends %d bytes. String <%s>\n", Rc, buf);
  else printf ("Error %d/%d: <%s>\n", Rc, errno, Tcp4uErrorString (Rc));

  if (lpSock!=NULL)   UdpCleanup (lpSock);
  Tcp4uCleanup();
return 0;
}


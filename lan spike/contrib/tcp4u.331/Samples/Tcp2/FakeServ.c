/* ------------------------------------------------------------------------- */
/* A sample program for Tcp4u.                                               */
/* This app waits on port HTTP (80) accepts any connections and              */
/* prints client's request.                                                  */
/* If you run this program under Unix, you must either have root             */
/* privilege or change the listening port (ie use 8080)                      */
/*                                                                           */
/*                                                                           */
/* Please read file ../build.txt before compiling this sample                */
/*                                                                           */
/* ------------------------------------------------------------------------- */

#include <stdio.h>
#include <tcp4u.h>


static char szBuf[4096];

main ()
{
SOCKET LSock, CSock;
unsigned short usPort=80;
int Rc;
char szVer [128];

  Tcp4uVer (szVer, sizeof szVer);
  printf ("Using %s\n", szVer);

  /* --- prepare a listening connection */
  Tcp4uInit ();
  Rc = TcpGetListenSocket (& LSock, "http", & usPort, 1);
  if (Rc!=TCP4U_SUCCESS)
  {
     printf ("%s\n", Tcp4uErrorString(Rc));
     return 0;
  }
  /* --- now listen at port */
  Rc = TcpAccept (&CSock, LSock, 0);
  while (Rc>=TCP4U_SUCCESS)
  {
    /* now the application code. Note that you can fork or creat */
    /* a new thread in order to create a multi-user server       */
    /* --- in this app, we listen at the opened socket */
    Rc = TnReadLine (CSock, szBuf, sizeof szBuf, 60, HFILE_ERROR);
    if (Rc>=TCP4U_SUCCESS)  printf ("%s\n", szBuf);
  }
  TcpClose (& CSock);
  Tcp4uCleanup ();
return 0;
}  /* main */   


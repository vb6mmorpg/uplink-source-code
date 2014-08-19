
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                                                                           */
/*   M i n i _ F t p                                                         */
/*                                                                           */
/*       Sample program for libtcp4u.a                                       */
/* This program establishes a connection with a FTP server (given as         */
/* argument), waits for its incoming string and displays it.                 */
/*                                                                           */
/*                                                                           */
/* Please read file ../build.txt before compiling this sample                */
/*                                                                           */
/*                                                             by Ph. Jounin */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <tcp4u.h>

#define NO_PRIVILEGE (0==1)  /* FALSE */

char szBuf[10240];

/* ------------------------------------------------------ */
/* A dialogue -> Sends commands then waits for its answer */
/* exits on error                                         */
/* ------------------------------------------------------ */
void FtpDialogue (SOCKET FtpSock, const char *szCmd)
{
int Rc;
  if (szCmd!=NULL) 
  {
     printf  (" --> %s\n", szCmd);
     Rc=TnSend (FtpSock, szCmd, NO_PRIVILEGE, HFILE_ERROR);
     if (Rc!=TCP4U_SUCCESS)
     {
        printf ("Send: error %d: <%s>\n", Rc, Tcp4uErrorString (Rc));
        exit(0);
     }
  }
  Rc = TnGetAnswerCode (FtpSock, szBuf, sizeof szBuf, 30, HFILE_ERROR);
  if (Rc<100)
  {
    printf ("Recv: error %d: <%s>\n", Rc, Tcp4uErrorString (Rc));
    exit(0);
  }  /* Recv Error */
  else   puts (szBuf);
} /* FtpDialogue */


/* ------------------------------------------------------ */
/* Main: Establish connection to an FTP server and talk a */
/* while with it                                          */
/* ------------------------------------------------------ */
int main (int argc, char *argv[])
{
int    Rc;
SOCKET FtpSock;
char szVer [128];

  Tcp4uVer (szVer, sizeof szVer);
  printf ("Using %s\n", szVer);

   Tcp4uInit();
   if (argc!=2)  { puts ("Usage: mini_Ftp <serveur>\n"); exit(0); }
   Rc = TcpConnect (&FtpSock, argv[1], "ftp", NULL);
   if (Rc!=TCP4U_SUCCESS)
   {
     printf ("Connect: error %d: <%s>\n", Rc, Tcp4uErrorString (Rc));
     exit(0);
   } /* Connect Error */
   FtpDialogue (FtpSock, NULL);
   FtpDialogue (FtpSock, "HELP");
   FtpDialogue (FtpSock, "QUIT");
   TcpClose (&FtpSock);
   Tcp4uCleanup();
return 0;
} /* main */




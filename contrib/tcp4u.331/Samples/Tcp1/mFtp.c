
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                                                                           */
/*   M i n i _ F t p                                                         */
/*                                                                           */
/* Sample program for libtcp4u.a                                             */
/*                                                                           */ 
/* This program establishes a connection to an existing FTP server           */
/* then prints the welcome frame sent by the server.                         */
/* This sample can be compiled as a QuickWin application under               */
/* Windows 3.1x. However, error string is displayed only in large            */
/* model.                                                                    */
/*                                                                           */
/* Please read file ../build.txt before compiling this sample                */
/*                                                                           */
/*                                                             by Ph. Jounin */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <tcp4u.h>

#undef FALSE
#define  FALSE            (0==1)
#define  NO_PRIVILEGE     FALSE 

/* FTP replies */
#define  POSITIVE_PREL   '3'
#define  POSITIVE_REPLY  '2'


int main (int argc, char *argv[])
{
int    Rc;
int    Ark=1;
SOCKET FtpSock;
char   szStr[2048];
char   szVer [128];

   /* display info on this library */
  Tcp4uVer (szVer, sizeof szVer);
  printf ("Using %s\n", szVer);

   Rc = Tcp4uInit();
   if (Rc!=TCP4U_SUCCESS)  {puts ("can not init Tcp4u"); exit(0); }
   if (argc>2 && strcmp (argv[Ark], "-v")==0) 
        Ark++,  Tcp4uEnableLog (LOG4U_DUMP);
   if (argc-Ark!=1)  { puts ("Usage: mFtp [-v] <server>\n"); exit(0); }

   /* connet to server and wait its reply */
   Rc = TcpConnect (&FtpSock, argv[Ark], "ftp", NULL);
   if (Rc==TCP4U_SUCCESS)
      Rc = TnReadMultiLine (FtpSock, szStr, sizeof szStr, 20, HFILE_ERROR);
   if (Rc>=TCP4U_SUCCESS)
           printf ("Server answers : \n%s\n", szStr);
   else    printf ("error %d: %s\n", Rc, Tcp4uErrorString(Rc));

   /* on success try to log as anonymous */
   if (Rc>=TCP4U_SUCCESS  &&  szStr[0]==POSITIVE_REPLY)
   {
       TnSend (FtpSock, "USER anonymous", FALSE, HFILE_ERROR);
       Rc = TnReadMultiLine (FtpSock, szStr, sizeof szStr, 20, HFILE_ERROR);
       if (Rc==TCP4U_SUCCESS)  printf ("%s\n", szStr);
       if (Rc>=TCP4U_SUCCESS  && szStr[0]==POSITIVE_PREL)
       {
           TnSend (FtpSock, "PASS guest@", FALSE, HFILE_ERROR);
           Rc = TnReadMultiLine (FtpSock, szStr, sizeof szStr, 20, HFILE_ERROR);
          if (Rc==TCP4U_SUCCESS)  printf ("%s\n", szStr);
       }
   }


   TcpClose (&FtpSock);
   Tcp4uCleanup();
return 0;
} /* main */




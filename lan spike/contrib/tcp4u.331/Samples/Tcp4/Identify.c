
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                                                                           */
/*   Identify                                                                */
/*                                                                           */
/*       Sample program for libtcp4u.a                                       */
/* This program display host name and IP address                             */
/*                                                                           */
/* Please read file ../build.txt before compiling this sample                */
/*                                                                           */
/*                                                             by Ph. Jounin */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <tcp4u.h>


int main (void)
{
int    Rc;
char   szVer [128];
char   szName [289];
DWORD  dwIP;
unsigned char  *pIP = & dwIP;   /* How to display it */

   Tcp4uVer (szVer, sizeof szVer);
   printf ("Using %s\n", szVer);
   Tcp4uInit();
 
   Rc = TcpGetLocalID (szName, sizeof szName, & dwIP);
   printf ("This server is <%s>, Address %u.%u.%u.%u\n",
            szName,
            pIP[0], pIP[1], pIP[2], pIP[3]);

   Tcp4uCleanup();
return 0;
} /* main */




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*   H t t p _ G u e s s                                           */
/*                                                                 */
/* Sample program for libtcp4u.a                                   */
/*                                                                 */ 
/* This program accepts a list of IP address, try to establish     */
/* a connection on HTTP port then display the name of remote       */
/* HTTP server. Work also through a firewall.                      */  
/*                                                                 */
/*                                                                 */
/* Please read file ../build.txt before compiling this sample      */
/*                                                                 */
/*                                                                 */
/*                                                   by Ph. Jounin */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <string.h>

#include <tcp4u.h>
#include <http4u.h>

#ifndef TRUE
#  define TRUE  (0==0)
#  define FALSE (0==1)
#endif


BOOL bVerbose=FALSE;

#define FIREWALL NULL   /* or something like "firewall.com:8080" */

/* Do it for the szServer server */
int Http_Gs (const char *szServer, int Ark)
{
int    Rc;
char   szHead[2048], *p, *q;
char   szURL[128], szHeaderFile[128];

   Tcp4uInit();
   if (bVerbose)  Tcp4uEnableLog (LOG4U_DUMP);
   
   sprintf (szURL, "http://%s/", szServer);
   sprintf (szHeaderFile, "Header.%d", Ark);
   if (bVerbose)  printf ("%s  ", szURL), fflush (stdout);
   memset (szHead, 0, sizeof szHead);
   Rc=HttpGetFileEx (szURL, FIREWALL,               /* URL and its proxy */
		     	     NULL,           /* do not keep HTML file            */ 
			         NULL,           /* do not keep Header into a file   */
			         NULL, 0,        /* Callback with its parameter      */
			         NULL, 0,        /* do not keep HTML data in memory  */
			         szHead, sizeof szHead);  /* but Header              */
   if (bVerbose)  
      printf (" --> %s\n", szHead );
   if (Rc==HTTP4U_SUCCESS)
   {
     p = strstr (szHead, "Server:");
     if (p!=NULL)    /* Server string has been found */
     {
       q = strchr (p, '\n'); 
       if (q!=NULL)  *q=0;  /* print only Server string */
       printf ("%-.20s: %s\n", szServer, p);
     }
     else 
     {
       fprintf (stderr, "%-.20s: Server not specified\n", szServer);
     }
   }
   else 
       fprintf (stderr, "%-.20s: %d->%s\n", szServer, Rc,Http4uErrorString(Rc));
       
    Tcp4uCleanup();   
return Rc;
} /* Http_Gs */


/* --------------------------------------------------------------- */
/* Main loop : analyze parameters and call Http_Gs for each server */
/* --------------------------------------------------------------- */
int main (int argc, char *argv[])
{
int Ark=0;
char szVer [128];

  Tcp4uVer (szVer, sizeof szVer);
  printf ("Using %s\n", szVer);

   if (argc>=2  &&  strcmp (argv[1], "-v")==0)  Ark++, bVerbose=TRUE;
   if (argc-Ark<=1)  
   { 
       puts ("Usage:\tHttp_gs [-v] <server> ... \n\tRetrieve server types"); 
       exit(0); 
   }
   
   while (++Ark < argc)
   {
     Http_Gs (argv[Ark], Ark);
   }
return 0;
} /* main */




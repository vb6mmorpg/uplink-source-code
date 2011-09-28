/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Test for Http4u                                                           */
/* This sample app gets all URLs specified into a file which is passed as    */ 
/* parameter. It can be compiled as a Windows 3.1x quickwin application.     */
/* However, callback code is rather nasty.                                   */
/*                                                                           */
/* Please read file ../build.txt before compiling this sample                */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <tcp4u.h>
#include <http4u.h>

#ifndef TRUE
#   define TRUE (1==1)
#endif

/* print help */
void Usage (void)
{
  printf ("Usage: http_get [-d] <file>\n");
  printf ("This sample app gets all URLs specifed into file\n");
  exit(0);
} /* Usage */


BOOL CALLBACK MyCbk(long lBytesTransferred, long lTotalBytes, long lUserValue,
                    LPCSTR sDummy, int nDummy)
{
#if defined (_WINDOWS) && !defined (WIN32)
   /* special printf format and casts for Windows 3.1x. */
   /* Does not work with the DLL  (womeone knows why ?) */
    printf ("%8ld/%8ld\t%Fs\r", lBytesTransferred, lTotalBytes, 
            lUserValue==0 ? (LPCSTR) "NULL" :  (LPCSTR)lUserValue);  
#else
    printf ("%8d/%8d\t%s\r", (int) lBytesTransferred, (int) lTotalBytes, 
            lUserValue==0 ? "NULL" :  (LPSTR) lUserValue);  
#endif      
    fflush (stdout);
return TRUE;
} /* MyCbk */


/* Process one URL */
int ProcessLine (const char *szURL, const char *szProxyURL)
{
int   Rc;
char *p;
char  szResponse[1024]; 
LPCSTR Prox;

  /* for MSVC 16 bit: automatic cast will not work -> GPF */
  Prox =  (szProxyURL==NULL) ?NULL : szProxyURL;
 
  p = strrchr (szURL, '/');  if (p==NULL)  return HTTP4U_BAD_URL;
  p++;
  szResponse [0] = 0;
  Rc=HttpGetFileEx(szURL,                    /* URL to be retrieved     */
                   Prox,                     /* the proxy  to be used   */
                   (*p==0 ? "Main.htm" : p), /* local file              */
                   NULL,                     /* forget headers          */
                   MyCbk,                    /* to be called            */
                   (long) (LPSTR)  szURL,     /* to be passed to MyCbk  */
                   szResponse, sizeof szResponse, /* errors rfom server */
                   NULL, 0);
  printf ("                                             ");
  printf ("                                            \r");
  printf ("%s: %s\n", szURL, szResponse[0]==0 ?  Http4uErrorString(Rc) :   szResponse);
return Rc;
} /* ProcessLine */


/* main function */
int main (int argc, char *argv[])
{
FILE *hF;
char  sz [1024], *p, *szProxyURL;
char szVer [128];

  Tcp4uVer (szVer, sizeof szVer);
  printf ("Using %s\n", szVer);

  if (argc==3 && argv[1][0]=='-' )
  {
     switch (argv[1][1])
     {
	case 'd' :  Tcp4uEnableLog (LOG4U_ALL); break;
	default  :  Usage();
     }
  }
  else if (argc!=2)  Usage ();   /* does not return */
  

  /* Necessary under Windows */
  Tcp4uInit();  
  /* set a five minutes timeout */
  Http4uSetTimeout (300);

  /* Open file and calls ProcessLine foreach line */  
  hF = fopen (argv[argc-1], "r");
  if (hF==NULL)   { printf ("Can not open %s\n", argv[argc-1]);  exit(0); }
  while (fgets (sz, sizeof sz, hF) != NULL) 
  {
     if (sz[0]==';'  || sz[0]=='#')  continue;
     p = strchr (sz, '\r');     if (p!=NULL)   *p = 0;
     p = strchr (sz, '\n');     if (p!=NULL)   *p = 0;
     /* search for Proxy */
     if (  (p = strchr (sz, ' '))==NULL  &&  (p = strchr (sz, '\t'))==NULL)  
	        szProxyURL=NULL;
     else
     {
         for ( szProxyURL = p ;  
              *szProxyURL!=0  && isspace(*szProxyURL) ; 
               szProxyURL++ );
         if (*szProxyURL==0)  szProxyURL=NULL;
	    *p=0 ;          /* end of string szURL */
     }
     ProcessLine (sz, szProxyURL);
  } /* until whole file is processed */
  fclose (hF);

  Tcp4uCleanup();       
return 0;
} /* main */


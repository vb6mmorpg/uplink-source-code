/*
 * Tcp4u v 3.31          Last Revision 27/06/1997  3.30
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    http4u_url.c
 * Purpose: everything which deals with URLs
 *
 *===========================================================================
 *
 * This software is Copyright (c) 1996-1998 by Philippe Jounin and Laurent Le Bras
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 *
 *
 *  If you make modifications to this software that you feel
 *  increases it usefulness for the rest of the community, please
 *  email the changes, enhancements, bug fixes as well as any and
 *  all ideas to me. This software is going to be maintained and
 *  enhanced as deemed necessary by the community.
 *
 *
 *             Philippe Jounin (ph.jounin@computer.org)
 */


#include "build.h"


#define  SLASH     '/'
#define  PERIOD    ':'
#define  HTTP_SERVICE  "http"
#define  HTTP_PORT      80


/* ------------------------------------------------------------------------- */
/* HttpIsValidURL: Retourne TRUE si l'URL est valide. De plus les differents */
/* composants de l'URL sont retournes a l'utilisateur                        */
/* ------------------------------------------------------------------------- */

BOOL API4U HttpIsValidURL 
                    (LPCSTR szURL, unsigned short far *lpPort, 
                     LPSTR  szService,   int  uServiceSize,
                     LPSTR  szHost,      int  uHostSize,
                     LPSTR  szFile,      int  uFileSize )
{
LPCSTR p, q;
int ServerLength;

   Tcp4uLog (LOG4U_HIPROC, "HttpIsValidURL");

    szHost[0]=0;
    /* default service : HTTP */
    Strcpyn (szService, HTTP_SERVICE, uServiceSize);

    /* if URL begins with "service:" do the job then forget it */
    p = Tcp4uStrIStr (szURL, "://");
    if (p!=NULL)
    {
       Strcpyn (szService, szURL, min ((int) (p - szURL + 1), uServiceSize) );
       szURL = p+1;
    }
    /* The same if URL begins with "//" */
    if (szURL[0]==SLASH  &&  szURL[1]==SLASH)   szURL+=2;

    /* URL is now either "Host:Port/File" or "Host/File" */
    for (p=szURL ; *p!=0  &&  *p!=SLASH ; p++);    if (*p==0)  p=NULL;
    if (szFile!=NULL)  Strcpyn (szFile, p==NULL ? "/" : p, uFileSize); 
    
    /* search for port which should be before File */
    if (p==NULL)  p = szURL + Strlen(szURL) - 1; /* last character */
    for (q=p ; q>szURL && *q!=':' ; q--);
    if (q==szURL)  q=NULL;  /* -> : not found */
   *lpPort = q==NULL ?  HTTP_PORT : Tcp4uAtoi (q+1);
 
    /* Host is to be copied */
    ServerLength =  p==NULL ? Strlen (szURL)  :
                        (q==NULL ? (p-szURL)    : (q-szURL) );  
    Strcpyn (szHost, szURL, min (ServerLength+1, uHostSize));
     
   Tcp4uLog (LOG4U_HIEXIT, "HttpIsValidURL");
return szHost[0]!=0; /* rejects if no host specified otherwise accepts anything */
} /* HttpIsValidURL */


#ifdef TEST
main (int argc, char *argv[])
{
char szService[10], szHost[10], szFichier[30]; /* short string to test limits */
unsigned short  usPort;
int Rc;

  /* ------------------------------init */
  memset (szService, 0, sizeof szService); 
  memset (szHost, 0, sizeof szHost); 
  memset (szFichier, 0, sizeof szFichier); 
  usPort = -1;
  /* ------------------------------init */

  Rc=HttpIsValidURL(argv[1], &usPort, szService, 10, szHost, 10, szFichier, 30);

  printf ("l'URL <%s> est-elle valide ? .... %s\n", 
          argv[1], Rc ? "oUi" : "Non");
  {
    printf ("Port    : %d\n", usPort);
    printf ("Serveur : <%s>\n", szHost);
    printf ("Service : <%s>\n", szService);
    printf ("Fichier : <%s>\n", szFichier);
  }
}
#endif

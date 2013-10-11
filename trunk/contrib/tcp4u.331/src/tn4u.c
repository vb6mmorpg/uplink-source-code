/*
 * Tcp4u v 3.31          Last Revision 04/03/1998  3.30.02
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    tn4u.c
 * Purpose: telnet based-functions
 *
 *===========================================================================
 *
 * This software is Copyright (c) 1996-1998 by Philippe Jounin
 *
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


/* ******************************************************************* */
/*                                                                     */
/* Partie II : Lecture des trames de type Telnet                       */
/*                                                                     */
/* ******************************************************************* */

/* ------------------------------------------------------------ */
/* TnReadLine : Lit une ligne de donnees (jusqu'à <LF>)         */
/*              retourne TN_ERROR, TN_TIMEOUT, TN_BUFFERFREED   */
/*              TN_OVERFLOW ou le nombre d'octets lus           */
/*              dans les deux derniers cas le buffer szBuf est  */
/*              renseigne. Si TN_SUCCESS il est <LF>-termine.   */
/* ------------------------------------------------------------ */

int API4U TnReadLine (SOCKET s,LPSTR szBuf,UINT uBufSize,UINT uTimeOut,HFILE hf)
{
int Rc;

   Tcp4uLog (LOG4U_PROC, "TnReadLine");
   Rc = InternalTnReadLine (s, szBuf, uBufSize, uTimeOut, hf);
   if (Rc==TN_SUCCESS)   Tcp4uDump (szBuf, Strlen(szBuf), DUMP4U_RCVD);
   Tcp4uLog (LOG4U_EXIT, "TnReadLine");
return Rc;
} /* TnReadLine */


int InternalTnReadLine (SOCKET s,LPSTR szBuf,UINT uBufSize,UINT uTimeOut,
                        HFILE hf)
{
int Rc;

   if (s==INVALID_SOCKET || uBufSize==0)  return TN_ERROR;  /* ??? */

   memset (szBuf, 0, uBufSize);
   Rc = InternalTcpRecvUntilStr (s, szBuf, & uBufSize, "\n", 1, 
                                 TRUE, uTimeOut, hf);
   if (Rc>=TCP4U_SUCCESS  &&  szBuf[uBufSize-1]=='\r')  
         szBuf[uBufSize-1]=0 ;   
   /* pas de translation des erreurs, timeout... */
return  (Rc>=TCP4U_SUCCESS) ?  TN_SUCCESS  : Rc;
} /* InternalTnReadLine */





/* ******************************************************************* */
/*                                                                     */
/* Partie III : Etage Telnet Haut                                      */
/*                                                                     */
/* ******************************************************************* */

/* ------------------------------------------------------------ */
/* TnSend : Formatage d'une trame au format Telnet (terminée    */
/*          par <RC><LF>                                        */
/*          Retour par TN_ERROR, ou TN_SUCCESS.                 */
/* ------------------------------------------------------------ */
int API4U TnSend (SOCKET s, LPCSTR szString, BOOL bHighPriority, HFILE hf)
{
int Rc, Z;
LPSTR p;

   Tcp4uLog (LOG4U_PROC, "TnSend");

   Z = Strlen (szString)+sizeof ("\r\n")-1;
   p = Calloc (Z+1, 1);
   if (p==NULL)  return TCP4U_INSMEMORY;
   Strcpy (p, szString);
   Strcat (p, "\r\n");
   Rc = InternalTcpSend (s, p, Z, bHighPriority, hf);
   if (Rc==TCP4U_SUCCESS)   Tcp4uDump (p, Z, DUMP4U_SENT);
   Free (p);
   Tcp4uLog (LOG4U_EXIT, "TnSend : return code %d", Rc);
return Rc==TCP4U_SUCCESS ?  TN_SUCCESS : TN_ERROR;
} /* TnSend */



/* ------------------------------------------------------------ */
/* TnReadMultiLine :  waits for a message from the remote peer  */
/*                    This message can contain several lines    */
/*                    As said in Telnet RFC, it depends on the  */
/*                    4th char.                                 */
/*                  - A TN_XX code is returned                  */
/* 20.02.1998: A bug has been found and fixed by Sergej Semenov.*/
/*                   The loop test was weak enough.             */
/* 04.03.1998: rewritten according to the FTP spec (RFC 959)    */
/*                    Should manage propoerly Text like :       */
/*                    123-First line                            */
/*                    Second line                               */
/*                        234 A line beginning with numbers     */
/*                    123 The last line                         */
/* ------------------------------------------------------------ */
int API4U TnReadMultiLine (SOCKET ctrl_skt, LPSTR szInBuf,
                           UINT uBufSize, UINT uTimeOut, HFILE hf)
{
int      Rc, TnRc;
unsigned nPos=0;
   
   Tcp4uLog (LOG4U_PROC, "TnReadMultiLine");

   if (szInBuf==NULL || uBufSize==0)  return TN_ERROR;
   memset (szInBuf, 0, uBufSize);

   do
   {
       /* add the <LF> character */
       if (szInBuf[nPos] != 0)        /* changed by Sergej */
       {    
           nPos+=Strlen (&szInBuf[nPos]);     /* pointer sur la fin de ligne */
       	   szInBuf[nPos++]='\n';
       }
       Rc = InternalTnReadLine (ctrl_skt, &szInBuf[nPos], 
                                uBufSize-nPos-1, uTimeOut,hf);
       /* exit on  - errors                                  */
       /*          - buffer full                             */
       /*          - 3 digits at the beginning of the string */
       /*            not followed by HYPHEN                  */
    }
    while (    nPos < uBufSize - 5  /* encore de la place (y compris pour \0) */
            && Rc>=TCP4U_SUCCESS    /* retour correct */
            && ! (isdigit (szInBuf[nPos]) &&  szInBuf[nPos+3] != '-' ) );

    /* supprime la derniere marque fin de ligne */
    if (Rc>=TCP4U_SUCCESS)
    {                       
       Rc = TCP4U_SUCCESS;
       nPos+=Strlen (&szInBuf[nPos]);
       if (szInBuf[nPos-1]=='\r')   szInBuf[nPos-1]=0;
    }
    /* translation du code de retour */
    switch (Rc)
    {
       case TCP4U_TIMEOUT :      TnRc =  TN_TIMEOUT;		break;
       case TCP4U_SUCCESS :      Tcp4uDump (szInBuf, nPos, DUMP4U_RCVD);
                                 TnRc =  TN_SUCCESS;     	break;
       case TCP4U_OVERFLOW :     Tcp4uDump (szInBuf, nPos, "Overflow");
                                 TnRc =  TN_OVERFLOW; 		break;
       case TCP4U_SOCKETCLOSED : TnRc =  TN_SOCKETCLOSED;	break;
       case TCP4U_CANCELLED    : TnRc =  TN_CANCELLED;		break;
       default :                 TnRc =  TN_ERROR;			break;
    }

   Tcp4uLog (LOG4U_EXIT, "TnReadMultiLine: return code %d", TnRc);
return TnRc;
} /* TnReadMultiLine */



/* ------------------------------------------------------------ */
/* TnSendMultiLine : Formatage d'une trame au format Telnet     */
/*          <LF> expanded into <RC><LF>                         */
/*          Retour par TN_ERROR, ou TN_SUCCESS.                 */
/* ------------------------------------------------------------ */
int API4U TnSendMultiLine (SOCKET s, LPCSTR szString, BOOL bEnd, HFILE hf)
{
int     Rc, Z;
LPCSTR  p;
LPSTR   pString;

   Tcp4uLog (LOG4U_PROC, "TnSendMultiLine");

   
   /* counts the number of <LF> char */
   for ( Z=0, p=szString ;   *p!=0  ;  p++, Z++ )
       if (*p=='\n')  Z++;

   /* put the string into a new writeable buffer */
   pString = Calloc (Z+sizeof ("\r\n"), 1);
   if (pString==NULL)   return TCP4U_INSMEMORY;
   for ( Z=0, p=szString ;  *p!=0 ;  p++, Z++ )
   {
       if (*p=='\n'  && (p>szString && *(p-1)!='\r')) 
          pString[Z++] = '\r';
       pString[Z] = *p;
   }
   if (bEnd)   
   {
        pString[Z++] = '\r'; 
        pString[Z++] = '\n';
   }
   pString[Z] = 0;
   /* Job has been done, send the string and exit */
   Rc = InternalTcpSend (s, pString, Z, FALSE, hf);
   if (Rc==TCP4U_SUCCESS)   
        Tcp4uDump (pString, Z, DUMP4U_SENT);
   Free (pString);
   Tcp4uLog (LOG4U_EXIT, "TnSendMultiLine : return code %d", Rc);
return Rc==TCP4U_SUCCESS ?  TN_SUCCESS : TN_ERROR;
} /* TnSendMultiLine */


/* ------------------------------------------------------------ */
/* TnGetAnswerCode :  Attend une réponse du serveur et rend le  */
/*                    code numérique (les 3 premiers octets)    */
/*                    renvoyé par le serveur.                   */
/*                  - Retourne un nombre entre 100 et 999 si la */
/*                    fonction s'est bien passée, sinon un code */
/*                    d'erreur TN_ERROR, TN_TIMEOUT...          */
/* ------------------------------------------------------------ */
int API4U TnGetAnswerCode (SOCKET ctrl_skt, LPSTR szInBuf,
                           UINT uBufSize, UINT uTimeOut, HFILE hf)
{
int      Rc;

   Tcp4uLog (LOG4U_PROC, "TnReadAnswerCode");
   
   Rc = TnReadMultiLine (ctrl_skt, szInBuf, uBufSize, uTimeOut, hf);
   if (Rc==TN_SUCCESS)		Rc = Tcp4uAtoi (szInBuf);
   
   Tcp4uLog (LOG4U_EXIT, "TnReadAnswerCode: return code %d", Rc);   
return Rc;
} /* TnGetAnswerCode */


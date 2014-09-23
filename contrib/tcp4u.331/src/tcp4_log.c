/*
 * Tcp4u v 3.31         Last Revision 08/12/1997  3.31-01
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    tcp4_log.c
 * Purpose: Some logging / debugging
 *
 *===========================================================================
 *
 * This software is Copyright (c) 1996-1998 by Philippe Jounin
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
#include <stdarg.h>


/* using a static integer is a bad choice for a DLL, since each app */
/* shares the same variable. However is will be boring to keep a    */
/* context for each app.                                            */

static unsigned uMaskTrace = 0;


/* ------------------------------------------------------------------ */
/* fonctions de log                                                   */
/* ------------------------------------------------------------------ */

void API4U Tcp4uEnableLog (unsigned uMask)
{
  uMaskTrace = uMask;
} /* Tcp4uEnableLog */


void Tcp4uLog (unsigned uLevel, LPCSTR fmt, ...)
{
va_list marker;
char szBuf[512];

    if ( (uMaskTrace & uLevel) == 0)  return;

	Sprintf (szBuf, "Proc %d\t", GetCurrentTask ());
    switch (uLevel)
    {
       case LOG4U_ERROR :
    	    Sprintf (szBuf + Strlen (szBuf), "Error %d during ", 
                     WSAGetLastError());
            break;
       case LOG4U_HIPROC : 
       case LOG4U_PROC : 
            Sprintf (szBuf + Strlen (szBuf), "entering ");
            break;
       case LOG4U_DBCALL : 
       case LOG4U_CALL : 
            Sprintf (szBuf + Strlen (szBuf), "calling ");
            break;
       case LOG4U_INTERN :
            Sprintf (szBuf + Strlen (szBuf), "internal ");
            break;
       case LOG4U_HIEXIT : 
       case LOG4U_EXIT : 
            Sprintf (szBuf + Strlen (szBuf), "exiting ");
            break;
    }        
    va_start (marker, fmt);
	Vsprintf (szBuf + Strlen (szBuf), fmt, marker);
    va_end (marker);

    Strcat (szBuf, SYSTEM_EOL);
    OutputDebugString (szBuf);
} /* Tcp4uLog */


/* -------------------------------------------------------------- */
/* dump a binary or text frame. The output is the debug window    */
/* for Windows system and stderr for unix                         */
/* The code is a port of the xdump function from the cmu snmp lib */
/* Ajout du cast (unsigned char)                                  */
/* -------------------------------------------------------------- */
void API4U Tcp4uDump (LPCSTR cp, int nLen, LPCSTR szPrefix)
{
int   col, count, nPos;
char  szLine [128];
static const char tCvtHex[] = "0123456789ABCDEF";

    /* dump enabled ? */ 
    if ( (uMaskTrace & LOG4U_DUMP) == 0)  return;

    if (nLen==0)    /* Empty message -> has to be dumped */
    {
        if (szPrefix!=NULL)   Strcpyn  (szLine, szPrefix, 20);
        else                  szLine[0]=0;
        Strcat (szLine, " Empty Message");
        Strcat (szLine, SYSTEM_EOL);
        OutputDebugString (szLine);
        return;        
    }

    count = 0;
    while (count < nLen)
    {
        if (szPrefix!=NULL)   Strcpyn  (szLine, szPrefix, 20);
        else                  szLine[0]=0;
        nPos = Strlen (szLine);
        szLine[nPos++] = ' ';

        for (col = 0 ;   count + col < nLen   &&   col < 16 ;   col++)
        {
            if (col == 8)   szLine[nPos++] = '-', szLine[nPos++] = ' ' ;
	        szLine [nPos++] = tCvtHex [(unsigned char ) cp[count + col] >> 4];
	        szLine [nPos++] = tCvtHex [(unsigned char ) cp[count + col] & 0x0F];
	        szLine [nPos++] = ' ';
        }

        while(col++ < 16)      /* pad end of buffer with zeros */
        { 
            if (col == 8)  szLine[nPos++] = ' ', szLine[nPos++] = ' ';
            szLine[nPos++] = ' ';
            szLine[nPos++] = ' ';
            szLine[nPos++] = ' ';
        }
        szLine[nPos++] = ' ';
        szLine[nPos++] = ' ';

        for (col = 0;  count + col < nLen && col < 16 ;   col++) 
        {
	       szLine[nPos++] = isprint(cp[count + col]) ? cp[count + col] : '.';
        }
	    Strcpy (& szLine[nPos], SYSTEM_EOL);
        OutputDebugString (szLine);
        
        count += col;
    } /* while buffer nor printed */

} /* Tcp4uDump */


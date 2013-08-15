/*
 * Tcp4u v 3.31        Last Revision 06/06/1997  3.1
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    tcp4u_err.c
 * Purpose: version and error management 
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


/* ------------------------------------------------------------------ */
/* Compataibilite avec les anciennes fonctions                        */
/* ------------------------------------------------------------------ */
#ifdef _WINDOWS
  int API4U Tcp4wVer (LPSTR sz, UINT uSize)   { return Tcp4uVer(sz,uSize);   }
  LPSTR API4U Tcp4wErrorString (int Rc)       { return Tcp4uErrorString(Rc); }
#endif
#ifdef UNIX 
  int API4U Tcp4uxVer(LPSTR sz, UINT uSize)   { return Tcp4uVer(sz,uSize);   }
  LPSTR API4U Tcp4uxErrorString (int Rc)      { return Tcp4uErrorString(Rc); }
#endif



/* ------------------------------------------------------------------ */
/* Informations sur la version                                        */
/* ------------------------------------------------------------------ */
static LPCSTR szTcp4uVersion = 

#ifdef _WINDOWS
#  ifdef _WIN32
#     ifdef  TCP4W_DLL
                  "@(#)DLL Tcp4u Win32 by Ph.Jounin Version 3.31";
#     else
                  "@(#)Lib Tcp4u Win32 by Ph.Jounin Version 3.31";
#     endif /* DLL ? */
#  else
#     ifdef  TCP4W_DLL
                  "@(#)DLL Tcp4u Win16 by Ph.Jounin Version 3.31";
#     else
                  "@(#)Lib Tcp4u Win16 by Ph.Jounin Version 3.31";
#     endif /* DLL ? */                  
#  endif /* WIN32 */
#endif /* _WINDOWS */
#ifdef UNIX
                  "@(#)Tcp4u Unix by Ph.jounin Version 3.31";
#endif /* UNIX */

int   nTcp4uVersion  = 0x0331;


/* ------------------------------------------------------------------ */
/* La fonction les retournant                                         */
/* ------------------------------------------------------------------ */

int API4U Tcp4uVer (LPSTR szInfo, UINT uBufSize)
{
  if (szInfo!=NULL) Strcpyn(szInfo, szTcp4uVersion + 4, uBufSize);
return nTcp4uVersion;
} /* Tcp4uVer */



/* ------------------------------------------------------ */
/* Fonction:                                              */
/* Retourne un message texte donnant la cause de l'erreur */
/* Note: Certains messages sont specifiques a la version  */
/*       Windows... Peu importe                           */
/* ------------------------------------------------------ */

struct S_Tcp4Err
{
   int   Tcp4ErrCode;
   LPSTR Tcp4ErrString;
}; /* struct S_Tcp4Err */

static struct S_Tcp4Err sTcp4Err [] =
{
  { TCP4U_SUCCESS,         (LPSTR) "Successful call"                         },
  { TCP4U_ERROR,           (LPSTR) "Network error"                           },
  { TCP4U_TIMEOUT,         (LPSTR) "Timeout in recv or accept"               },
  { TCP4U_HOSTUNKNOWN,     (LPSTR) "Reference to Unknown host"               },
  { TCP4U_NOMORESOCKET,    (LPSTR) "All sockets has been used up"            }, 
  { TCP4U_NOMORERESOURCE,  (LPSTR) "No more free resource"                   },
  { TCP4U_CONNECTFAILED,   (LPSTR) "Connect function has failed"             },
  { TCP4U_UNMATCHEDLENGTH, (LPSTR) "TcpPPRecv : Error in length"             },
  { TCP4U_BINDERROR,       (LPSTR) "Bind failed (Task already started or attempt to open privileged socket )"     },
  { TCP4U_OVERFLOW,        (LPSTR) "Overflow during TcpPPRecv/TcpRecvUntilStr"},
  { TCP4U_EMPTYBUFFER,     (LPSTR) "TcpPPRecv receives 0 byte"               },
  { TCP4U_CANCELLED,       (LPSTR) "Call cancelled by signal or TcpAbort"    }, 
  { TCP4U_INSMEMORY,       (LPSTR) "Not enough memory to perform function"   },
  { TCP4U_BADPORT,         (LPSTR) "Bad port number or alias"                },
  { TCP4U_SOCKETCLOSED,    (LPSTR) "Host has closed connection"              },
  { TCP4U_FILE_ERROR,      (LPSTR) "A file operation has failed"             },
}; /* sTcp4Err */

LPSTR API4U Tcp4uErrorString (int Rc)
{
int Idx;
   Tcp4uLog (LOG4U_PROC, "Tcp4uErrorString");

   for ( Idx=0 ;  
         Idx<SizeOfTab(sTcp4Err) && sTcp4Err[Idx].Tcp4ErrCode!=Rc;  
         Idx++ );
   Tcp4uLog (LOG4U_EXIT, "Tcp4uErrorString");
return Idx>= SizeOfTab(sTcp4Err) ?   (LPSTR) "Not an Tcp4 return code" :
                                      sTcp4Err[Idx].Tcp4ErrString;
} /* Tcp4(w/ux)ErrorString */

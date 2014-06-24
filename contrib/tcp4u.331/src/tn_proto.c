/*
 * Tcp4u v 3.31          Last Revision 27/02/1998  3.30
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    tn_proto.c
 * Purpose: telnet based-protocol transactions
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


/* ------------------------------------------------------------------------- */
/* This function does the dirty job of any protocol a la telnet              */
/* it sends an order accepting variable length parameter, then waits for the */
/* response. The response is compared with an array if strings passed by the */
/* calling function. The result is the matching integer if any or            */
/* TN_UNEXPECTED                                                             */
/* ------------------------------------------------------------------------- */
int API4U TnProtoExchange (SOCKET s, 
                     LPCSTR szCommande,
                     LPSTR  szResponse, 
                     UINT uBufSize, 
                     TNPROTOEXCHG_CBK TnProtoRecv,
                     struct S_TnProto far *tTranslation, 
                     int    nTabSize,
                     BOOL   bCaseCmp,
                     UINT   uTimeout,
                     HFILE  hLogFile)
{
int     Rc;
int     Ark;
int (far * fCompare) (LPCSTR, LPCSTR, size_t);
#define XX_RETURN(x)  { \
    Tcp4uLog (LOG4U_EXIT, "TnReadAnswerCode: return code %d", x); \
     return (x); }


   Tcp4uLog (LOG4U_PROC, "TnProtoExchange");

   if (szCommande != NULL)
   {
     Rc = TnSend (s, szCommande, FALSE, hLogFile);
     if (Rc != TN_SUCCESS)   XX_RETURN (Rc);
   }

   /* waits for answer. NOte the final test */
   Rc = TnProtoRecv (s, szResponse, uBufSize, uTimeout, hLogFile);
   if (Rc<TN_SUCCESS)  XX_RETURN (Rc);

   /* choose the function for string compare */
   fCompare = bCaseCmp ? strncmp : Tcp4uStrncasecmp;

   /* translate the code */
   for ( Ark=0 ;  Ark<nTabSize &&  tTranslation[Ark].szString!=NULL;  Ark++)
   {
       if (fCompare (szResponse, tTranslation[Ark].szString, Strlen (tTranslation[Ark].szString)) == 0)
            XX_RETURN (tTranslation[Ark].iCode);
   }
XX_RETURN (TN_UNEXPECTED);
} /* TnProtoExchange */


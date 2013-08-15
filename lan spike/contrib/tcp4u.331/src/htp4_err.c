/*
 * Tcp4u v 3.31          Last Revision 06/06/1997  3.10
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    http4u.c
 * Purpose: manage http 1.0 protocol
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

 

/*******************************
 * Http4u miscellaneous define
 *******************************/
#ifdef UNIX
#  define trace 1
#else
#  define trace 0
#endif


/*******************************
 * Http4u global variables
 *******************************/
struct S_http4u_errors 
{
  enum HTTP4_RETURN_CODE eErr;
  LPCSTR                 sErr;
} 
tHttp4uErrors[] = 
{
 {HTTP4U_BAD_URL,        "Url does not conform to Http protocol.",  },
 {HTTP4U_TCP_CONNECT,    "can't reach the http server.",            },
 {HTTP4U_HOST_UNKNOWN,   "can not resolve host address",            },
 {HTTP4U_TCP_FAILED,     "call to Tcp4u library failed.",           },
 {HTTP4U_FILE_ERROR,     "file stream error.",                      },
 {HTTP4U_INSMEMORY,      "insuffisant memory.",                     }, 
 {HTTP4U_BAD_PARAM,      "bad parameter in the function call.",     },
 {HTTP4U_OVERFLOW,       "user buffer so little .",                 },
 {HTTP4U_CANCELLED,      "tranfser aborted.",                       },
 {HTTP4U_NO_CONTENT,     "data is empty.",                          },
 {HTTP4U_MOVED,          "request moved to other server.",          },
 {HTTP4U_BAD_REQUEST,    "bad request received by the server.",     },
 {HTTP4U_FORBIDDEN,      "request forbidden.",                      },
 {HTTP4U_NOT_FOUND,      "destination file not found.",             },
 {HTTP4U_PROTOCOL_ERROR, "protocol error.",                         },
 {HTTP4U_UNDEFINED,      "miscellaneous error.",                    },
 {HTTP4U_TIMEOUT,        "timeout in TCP dialog.",                  },
 {HTTP4U_SUCCESS,        "Successful call."                         },
};


/*######################################################################
 *##
 *## NAME:  Http4uErrorString
 *##
 *## PURPOSE: Writes a message explaining a function error
 *##
 *####################################################################*/
LPCSTR  API4U Http4uErrorString(
                         int msg_code  /* le code d'erreur de Http4U */
                        )
{
int Idx;
LPCSTR p;

   Tcp4uLog (LOG4U_HIPROC, "Http4uErrorString");

   for ( Idx=0 ;  
         Idx<SizeOfTab(tHttp4uErrors) && tHttp4uErrors[Idx].eErr!=msg_code;  
         Idx++ );

   p = Idx>= SizeOfTab(tHttp4uErrors) ?   (LPSTR) "Not an Http4u return code" :
                                             tHttp4uErrors[Idx].sErr;

   Tcp4uLog (LOG4U_HIEXIT, "Http4uErrorString");
return p;
} /* END Http4uErrorString */

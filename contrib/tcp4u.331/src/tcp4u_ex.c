/*
 * Tcp4u v 3.31        Last Revision 27/06/1997  3.10
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    tcp4u_ex.c
 * Purpose: Add-on to Tcp4u library
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


#ifndef trace
#define trace 1
#endif

/*######################################################################
 *##
 *## NAME:   TCPRecvUntilClosedEx
 *##
 *## PURPOSE: Receive Tcp data until socket connection closed by server
 *##
 *####################################################################*/
int API4U TcpRecvUntilClosedEx 
           (SOCKET   far  *pCSock,          /* socket comm.*/
            LPCSTR         szLocalFile,     /* local file transfer */
            TRANSFER_CBK  CbkTransmit,      /* callback transfer */
            unsigned      uTimeout,              /* timeout */
            unsigned int  uBufSize,              /* buffer transfer size */
            long          lUserValue,            /* user value */
            long          lTotalBytes)           /* */
{
int     Rc;
long    lBytesTransferred = 0;
HFILE   hFile = HFILE_ERROR;
LPSTR   sBufRead = NULL;

#define XX_RETURN(a)   {\
                       if (hFile != HFILE_ERROR)  Close(hFile);\
                       if (sBufRead != NULL) Free(sBufRead);\
					   Tcp4uLog (LOG4U_EXIT, "TcpRecvUntilClosedEx, return code %d", a);  \
                       return a;\
                       }

   Tcp4uLog (LOG4U_PROC, "TcpRecvUntilClosedEx");

 /* open user local file */
 if (szLocalFile!=NULL  &&  (hFile=Open(szLocalFile, WRITE_CR)) == HFILE_ERROR) 
 {
    /* open failed */
    XX_RETURN (TCP4U_FILE_ERROR);
 }

  /* allocate buffer */
  if ( (sBufRead = Calloc(uBufSize<128 ? 128 : uBufSize,1)) == NULL) 
  {
    XX_RETURN(TCP4U_INSMEMORY);
  }

  /* first call to user callback */
  if (CbkTransmit != NULL && (*CbkTransmit)(lBytesTransferred,
                                         lTotalBytes,
                                         lUserValue,
                                         sBufRead,
                                         0) == FALSE) 
  {
     /* user stop order */
     XX_RETURN (TCP4U_CANCELLED);
  }

  /* read data from http server */
  do 
  {
     /* read one data frame */
     Rc=TcpRecv(*pCSock,
                sBufRead, /* adress */
                uBufSize,
                uTimeout,
                HFILE_ERROR);
     if (Rc >= TCP4U_SUCCESS) 
     {
        /* write to the user local file --> Warning signed/unsigned mismatch */
        if (hFile != HFILE_ERROR  && Write(hFile, sBufRead, Rc) != Rc) 
        {
          XX_RETURN(TCP4U_FILE_ERROR);
        }

        lBytesTransferred += (long) Rc;

        /* appel du callback */
        if (CbkTransmit != NULL && (*CbkTransmit)(lBytesTransferred,
                                               lTotalBytes,
                                               lUserValue,
                                               sBufRead,
                                               Rc) == FALSE) {
          XX_RETURN (TCP4U_CANCELLED);
        }
     }
  } while (Rc >= TCP4U_SUCCESS); /* END do while */

  /* close all files and free allocated buffers */
  XX_RETURN(Rc == TCP4U_SOCKETCLOSED ? TCP4U_SUCCESS : Rc);

#undef XX_RETURN
} /* END TcpRecvUntilClosedEx */

/*
 * Tcp4u v 3.31         Creation 10/07/1997 Last Revision 16/10/1997  3.11
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    udp4u.h
 * Purpose: main functions for udp protocol management
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

#ifndef UDP4UX_API


#ifndef _WINSOCKAPI_	/* winsock.h has not been loaded -> unix */
#  include <sys/types.h>
#  include <netinet/in.h>
#endif

#ifdef __cplusplus  
extern "C" {            /* Assume C declarations for C++ */   
#endif  /* __cplusplus */   


struct sUdpData
{
   SOCKET                 UdpSock;        /* Socket descriptor           */
   struct  sockaddr_in    saSendAddr;     /* destination                 */
   struct  sockaddr_in    saRecvAddr;     /* last received frame header  */
   struct  in_addr        saFilter;       /* Fileter en reception        */
   BOOL                   bSemiConnected; /* received only from SendAddr */
}; /* struct sUdpData */

typedef struct sUdpData  UDPSOCK;
typedef UDPSOCK far *    LPUDPSOCK;

/* different modes */
#define    UDP4U_CLIENT    45
#define    UDP4U_SERVER    56


int API4U UdpInit (LPUDPSOCK far *pUdp, LPCSTR szHost, 
                   unsigned short uRemotePort, unsigned short uLocalPort);
int API4U UdpCleanup (LPUDPSOCK Udp);
int API4U UdpSend (LPUDPSOCK Udp,
                   LPCSTR sData,     int nDataSize,
                   BOOL   bHighPriority, HFILE hLogFile);
int API4U UdpRecv (LPUDPSOCK pUdp,  LPSTR sData, int nDataSize, 
                  unsigned uTimeOut, HFILE hLogFile);
int API4U UdpBind (LPUDPSOCK pUdp, BOOL bFilter, int nMode);
unsigned short API4U Udp4uServiceToPort (LPCSTR szService);


#ifdef __cplusplus     
}  /* End of extern "C" */   
#endif /* ifdef __cplusplus */

#define UDP4UX_API loaded
#endif /* ifndef UDP4UX_API */

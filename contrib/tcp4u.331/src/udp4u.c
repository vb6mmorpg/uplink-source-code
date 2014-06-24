/*
 * Tcp4u v 3.31         Last Revision 09/03/1997  3.31-00
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    udp4u.c
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

#include "build.h"

static char szWhat[] ="@(#)Udp4u by Ph. Jounin version 3.31";

/* ------------------------------------------------------------------------- */
/* Fonction de traduction Service -> port                                    */
/* ------------------------------------------------------------------------- */
unsigned short API4U Udp4uServiceToPort (LPCSTR szService)
{
unsigned short uPort = 0;
struct servent far *  lpServEnt;

  if (szService!=NULL)
  {
    Tcp4uLog (LOG4U_CALL, "getservbyname service %s", szService);
    lpServEnt = getservbyname (szService, "udp") ;
    if (lpServEnt==NULL)
       Tcp4uLog (LOG4U_ERROR, "getservbyname service %s", szService);
    else  uPort = ntohs (lpServEnt->s_port);
  }
return uPort;
} /* Udp4uServiceToPort */


/* ------------------------------------------------------------------------- */
/* initialisation UDP                                                        */
/* ------------------------------------------------------------------------- */
int API4U UdpInit (LPUDPSOCK far *pUdp, LPCSTR szHost, 
                   unsigned short uRemotePort, unsigned short uLocalPort)
{
int                   Rc;
SOCKET                UdpSock = INVALID_SOCKET;
struct  sockaddr_in   saSendAddr; /* structure identifiant le destinataire */
struct  sockaddr_in   saBindAddr; /* structure limitant les clients        */

  Tcp4uLog (LOG4U_PROC, "UdpInit, host %s", szHost);

  memset (& saBindAddr, 0, sizeof saBindAddr);
  memset (& saSendAddr, 0, sizeof saSendAddr); 
  /* --- 1. calcul de saSendAddr : RemoteService et szHost */
  saSendAddr.sin_family = AF_INET;
  saSendAddr.sin_port = htons (uRemotePort) ;
  if (szHost!=NULL)
  {
    saSendAddr.sin_addr = Tcp4uGetIPAddr (szHost);
    if (saSendAddr.sin_addr.s_addr==INADDR_NONE)   
    {
	     Tcp4uLog (LOG4U_ERROR, "UdpInit: host Unknown");
     	 return TCP4U_HOSTUNKNOWN;
    }
  } /* szHost is specified */
 
  /* --- autres champs de saBindAddr             */
  saBindAddr.sin_port = htons (uLocalPort);
  saBindAddr.sin_family = AF_INET;
  saBindAddr.sin_addr.s_addr = INADDR_ANY;

  /* --- Allocation de la socket */
  Tcp4uLog (LOG4U_CALL, "socket PF_INET SOCK_DGRAM");
  UdpSock = socket (PF_INET, SOCK_DGRAM, 0);
  if (UdpSock<0) 
  {
     Tcp4uLog (LOG4U_ERROR, "socket PF_INET SOCK_DGRAM");
     return TCP4U_NOMORESOCKET;
  }

  /* --- do a bind, thus all packets to be received will come on given port */
  Tcp4uLog (LOG4U_CALL, "bind to %s", inet_ntoa (saBindAddr.sin_addr));
  Rc = bind (UdpSock, (struct sockaddr far *) & saBindAddr, 
             sizeof(struct sockaddr));
  if (Rc<0)   
  { 
     Tcp4uLog (LOG4U_ERROR, "bind to %s", inet_ntoa (saBindAddr.sin_addr));
     CloseSocket (UdpSock);  
     return TCP4U_BINDERROR; 
  }

  /* --- Allocation de la structure */
  *pUdp = Calloc (sizeof (UDPSOCK), 1);
  if (*pUdp==NULL)  { CloseSocket (UdpSock);  return TCP4U_INSMEMORY; }
  (*pUdp)->UdpSock = UdpSock;
  (*pUdp)->saSendAddr = saSendAddr; 
  (*pUdp)->bSemiConnected = FALSE;  /* can receive data from any host */

   Tcp4uLog (LOG4U_EXIT, "UdpInit");
return TCP4U_SUCCESS;
} /* UdpInit */


/* ------------------------------------------------------------------------ */
/* UdpBind: Restrict socket to listen only from previous client             */
/* ------------------------------------------------------------------------ */
int API4U UdpBind (LPUDPSOCK pUdp, BOOL bFilter, int nMode)
{
   Tcp4uLog (LOG4U_PROC, "UdpBind Mode %s", 
             nMode==UDP4U_CLIENT ? "client" : "server");

   if (bFilter)
   {
      switch (nMode)
      {
         /* le client ne peut recevoir que de la machine serveur           */
         /* le serveur que depuis la machine d'ou il a deja recu des infos */
         /* en mode serveur, l'adresse de destination devient l'adresse    */
         /* du dernier host.                                               */
         case UDP4U_CLIENT  :   pUdp->saFilter = pUdp->saSendAddr.sin_addr ; 
                                break;
         case UDP4U_SERVER  :   pUdp->saFilter = pUdp->saRecvAddr.sin_addr;
                                pUdp->saSendAddr = pUdp->saRecvAddr;
                                break;
         default            :   return TCP4U_ERROR;
      } /* selon mode */
   } /* si filtre positionne */
   /* filtre ou pas filtre */
   pUdp->bSemiConnected = bFilter;
   
   Tcp4uLog (LOG4U_EXIT, "UdpBind");
return  TCP4U_SUCCESS;
} /* UdpBind */



/* ------------------------------------------------------------------------- */
/* destructeur : UdpCleanup                                                  */
/* ------------------------------------------------------------------------- */
int API4U UdpCleanup (LPUDPSOCK Udp)
{
int Rc;
  Tcp4uLog (LOG4U_PROC, "UdpCleanup sock %d", Udp->UdpSock);

  Tcp4uLog (LOG4U_CALL, "closesocket sock %d", Udp->UdpSock);
  Rc = CloseSocket (Udp->UdpSock);
  if (Rc!=0)
  {
       Tcp4uLog (LOG4U_ERROR, "closesocket sock %d", Udp->UdpSock);
       return TCP4U_ERROR;
  }
  Free (Udp);

  Tcp4uLog (LOG4U_EXIT, "UdpCleanup");
return TCP4U_SUCCESS;
}  /* UdpCleanup */


/* ------------------------------------------------------------------------- */
/* Envoi d'une trame UDP                                                     */
/* ------------------------------------------------------------------------- */
int API4U UdpSend (LPUDPSOCK Udp, LPCSTR sData, int nDataSize,
                   BOOL   bHighPriority, HFILE hLogFile)
{
int Rc;
  Tcp4uLog (LOG4U_PROC, "UdpSend sock %d, %d bytes to be sent ", 
            Udp->UdpSock, nDataSize);
  Rc = InternalUdpSend (Udp, sData, nDataSize, bHighPriority, hLogFile);
  if (Rc==TCP4U_SUCCESS)   Tcp4uDump (sData, nDataSize, DUMP4U_SENT);
  Tcp4uLog (LOG4U_EXIT, "UdpSend");
return Rc;
} /* UdpSend */

int InternalUdpSend (LPUDPSOCK Udp,LPCSTR sData, int nDataSize,
                     BOOL   bHighPriority, HFILE hLogFile)
{
int Rc;

  Tcp4uLog (LOG4U_CALL, "sendto host %s sock %d, %d bytes to be sent ", 
            inet_ntoa (Udp->saSendAddr.sin_addr), 
            Udp->UdpSock, nDataSize);
  Rc = sendto (  Udp->UdpSock,    
                 sData, nDataSize, 
                 bHighPriority ? MSG_OOB : 0, 
                 (struct sockaddr far *) &Udp->saSendAddr, 
                 sizeof Udp->saSendAddr);
  /* --- Analyser la valeur retounee */
  if (Rc<0)
  {
        Tcp4uLog (LOG4U_ERROR, "sendto host %s", inet_ntoa (Udp->saSendAddr.sin_addr) );
        return TCP4U_ERROR;
  }
  if (hLogFile!=HFILE_ERROR)  Write (hLogFile, sData, Rc);

return Rc==nDataSize ? TCP4U_SUCCESS : TCP4U_OVERFLOW;
} /* InternalUdpSend */


/* ------------------------------------------------------------------------- */
/* La fonction reciproque : UdpRecv                                          */
/* ------------------------------------------------------------------------- */
int API4U UdpRecv (LPUDPSOCK pUdp,  LPSTR sData, int nDataSize, 
                  unsigned uTimeOut, HFILE hLogFile)
{
int            Rc;
  Tcp4uLog (LOG4U_PROC, "UdpRecv sock %d, buffer %d bytes", pUdp->UdpSock, nDataSize);
  Rc = InternalUdpRecv (pUdp, sData, nDataSize, uTimeOut, hLogFile);
  if (Rc>=TCP4U_SUCCESS)  Tcp4uDump (sData, Rc, DUMP4U_RCVD);
  Tcp4uLog (LOG4U_EXIT, "UdpRecv %d bytes received", Rc);
return Rc;
} /* UdpRecv */
  

int InternalUdpRecv (LPUDPSOCK pUdp,  LPSTR sData, int nDataSize, 
                     unsigned uTimeOut, HFILE hLogFile)
{
int            Rc;
struct timeval TO;
fd_set         ReadMask;                                      /* select mask */
int            Len = sizeof (struct sockaddr_in);

  if (pUdp->UdpSock==INVALID_SOCKET)  return TCP4U_ERROR;
  /* bug 1 :  Timeout kept in Loop, should be decremented                  */
  /* (can be done by the select function)                                  */
  /* bug 2 : some frames are read even if they do not concern this process */
  TO.tv_sec = (long) uTimeOut;                               /* secondes */
  TO.tv_usec = 0;                                       /* microsecondes */
  do
  {
     FD_ZERO (& ReadMask);                         /* mise a zero du masque */
     FD_SET (pUdp->UdpSock, & ReadMask);  /* Attente d'evenement en lecture */
     /* s+1 normally unused but better for a lot of bugged TCP Stacks */
     Tcp4uLog (LOG4U_CALL, "select Timeout %d", uTimeOut);
     Rc = select (  pUdp->UdpSock+1, 
                  & ReadMask, NULL, NULL, 
                    uTimeOut==0? NULL: &TO);
     if (Rc<0)
     {
          Tcp4uLog (LOG4U_ERROR, "select");
          return  IsCancelled() ? TCP4U_CANCELLED : TCP4U_ERROR;
     }
     if (Rc==0)
     {
          Tcp4uLog (LOG4U_ERROR, "select: Timeout");
          return  TCP4U_TIMEOUT;            /* timeout en reception */
     } 
   
     Tcp4uLog (LOG4U_CALL, "recvfrom host %s", inet_ntoa (pUdp->saRecvAddr.sin_addr));
     Rc = recvfrom (pUdp->UdpSock, sData, nDataSize, 0, 
                   (struct sockaddr far *) & pUdp->saRecvAddr, & Len);
     if (Rc<0)
     {
        Tcp4uLog (LOG4U_ERROR, "recvfrom");
        return TCP4U_ERROR;
     }
  }
  while (   pUdp->bSemiConnected  
         && memcmp (& pUdp->saRecvAddr.sin_addr, 
                    & pUdp->saFilter, 
                     sizeof  pUdp->saFilter) != 0);

  if (hLogFile!=HFILE_ERROR)  Write (hLogFile, sData, Rc);
return Rc;
} /* InternalUdpRecv */



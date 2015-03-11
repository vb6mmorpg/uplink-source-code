/*
 * Tcp4u v 3.31     Created may 93 - Last Revision 20/10/1997  3.20
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    tcp4u.c
 * Purpose: main functions for tcp management
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

#define TCP4U_SENDTIMEOUT   3600l  /* one hour */




/* ******************************************************************* */
/*                                                                     */
/* Partie I :  constructeurs /Destructeurs                             */
/*                                                                     */
/*                                                                     */
/*       Tcp4uInit, Tcp4uCleanup                                       */
/*                                                                     */
/*                                                                     */
/* ******************************************************************* */

/* ------------------------------------------------------------------ */
/* Initialisation                                                     */
/* ------------------------------------------------------------------ */
int API4U Tcp4uInit (void)
{
int     Rc=0;

   Tcp4uLog (LOG4U_PROC, "Tcp4uInit");

   Skt4uInit ();
#  ifdef _WINDOWS
   {
   /* enregistrement aupres de Winsock.Dll */
    WSADATA WSAData;
      Tcp4uLog (LOG4U_CALL, "WSAStartup, Winsock 1.1 required");
      Rc = WSAStartup (MAKEWORD (1,1), & WSAData);
   }
#  endif
   Tcp4uLog (LOG4U_EXIT, "Tcp4uInit");
return Rc==0 ? TCP4U_SUCCESS : TCP4U_ERROR;
} /* Tcp4uInit */



/* ------------------------------------------------------------------ */
/* Destructeurs                                                       */
/* ------------------------------------------------------------------ */
int API4U Tcp4uCleanup (void)
{
int Rc;
   Tcp4uLog (LOG4U_PROC, "Tcp4uCleanup");
   Rc = Skt4uCleanup ();
   Tcp4uLog (LOG4U_EXIT, "Tcp4uCleanup");
return Rc;
} /* Tcp4wCleanup */



/* ------------------------------------------------------------------ */
/* WINDOWS : Fin de vie de la DLL                                     */
/* ------------------------------------------------------------------ */
#ifdef TCP4W_DLL
int API4U WEP (int nExitType)
{
   Tcp4uLog (LOG4U_PROC, "WEP");
   Skt4uCleanup ();
   nExitType=0; /* suppress warning */
   Tcp4uLog (LOG4U_EXIT, "WEP");
return 1; /* definition Windows */
} /* WEP */
#endif



/* ------------------------------------------------------------------ */
/* Aliases : Tcp4uxInit/Tcp4uxCleanup ;  Tcp4wInit/Tcp4wCleanup       */
/* ------------------------------------------------------------------ */
#ifdef _WINDOWS
  int API4U Tcp4wInit (void)      { return Tcp4uInit ();    }
  int API4U Tcp4wCleanup (void)   { return Tcp4uCleanup (); }
#endif
#ifdef  UNIX
  int API4U Tcp4uxInit (void)     { return Tcp4uInit ();    }
  int API4U Tcp4uxCleanup (void)  { return Tcp4uCleanup (); }
#endif


/* ******************************************************************* */
/*                                                                     */
/* Partie I :  Fonctions de bas niveau TCP                             */
/*                                                                     */
/*                                                                     */
/*      TcpAbort                                                       */
/*      TcpAccept                                                      */
/*      TcpConnect                                                     */
/*      TcpClose                                                       */
/*      TcpFlush                                                       */
/*      TcpGetListenSocket                                             */
/*      TcpRecv                                                        */
/*      TcpSend                                                        */
/*                                                                     */
/* ******************************************************************* */



/* ------------------------------------------------------------ */
/* TcpAbort:  Stoppe un appel bloquant                          */
/*            Retourne TCP4U_SUCCESS                            */
/* ------------------------------------------------------------ */
int API4U TcpAbort (void)
{
   Tcp4uLog (LOG4U_PROC, "TcpAbort");
  if (WSAIsBlocking ())  
  {
     Tcp4uLog (LOG4U_CALL, "WSACancelBlockingCall");
     WSACancelBlockingCall ();
  }
  else      Tcp4uLog (LOG4U_ERROR, "TcpAbort: No calling process to cancel");
  Tcp4uLog (LOG4U_EXIT, "TcpAbort");
return TCP4U_SUCCESS;
} /* TcpAbort */



/* ------------------------------------------------------------ */
/* TcpAccept :  itablissement d'une connexion TCP avec Timeout  */
/*              Retourne TCP4U_ERROR, TCP4U_TIMEOUT, TCP4U_SUCCESS */
/*              ou TCP4U_BUFFERFREED si pConnectSock n'existe plus */
/*              En cas de succes la variable pConnectSock est   */
/*              renseignee.                                     */
/* ------------------------------------------------------------ */
int API4U TcpAccept (SOCKET far *pCSock, SOCKET ListenSock, UINT nTO)
{
struct timeval     TO;         /* Time Out structure    */
fd_set             ReadMask;   /* select mask           */
SOCKET             ConSock;
struct sockaddr_in saSockAddr; /* specifications pour le Accept */
struct linger      sling = { TRUE, 5 };    /* 5-seconds timeout */
int                nAddrLen = sizeof saSockAddr;
int                Rc;

  Tcp4uLog (LOG4U_PROC, "TcpAccept. Listen socket %d, Timeout %d", ListenSock, nTO);
  /* prepare select */
  FD_ZERO (& ReadMask);    /* mise a zero du masque */
  FD_SET (ListenSock, & ReadMask);  /* Attente d'evenement en lecture */
  TO.tv_sec = (long) nTO;  /* secondes */
  TO.tv_usec = 0;          /* microsecondes */
  /* s+1 normally unused but better for a lot of bugged TCP Stacks */
  Tcp4uLog (LOG4U_CALL, "select on socket %d", ListenSock);
  Rc = select (1+ListenSock, & ReadMask, NULL, NULL, nTO==0 ? NULL : & TO);
  if (Rc<0)   
  {
     Tcp4uLog (LOG4U_ERROR, "select on socket %d", ListenSock);
     return  IsCancelled() ? TCP4U_CANCELLED : TCP4U_ERROR;  /* erreur reseau */
  }
  if (Rc==0)
  {
      Tcp4uLog (LOG4U_ERROR, "select on socket %d: Timeout", ListenSock);
      return  TCP4U_TIMEOUT;
  }

  Tcp4uLog (LOG4U_CALL, "accept on listen socket %d", ListenSock);
  ConSock = accept (ListenSock, (struct sockaddr far *) &saSockAddr, &nAddrLen);
  if (ConSock==INVALID_SOCKET)   
  {
     Tcp4uLog (LOG4U_ERROR, "accept on socket %d", ListenSock);
     return IsCancelled() ? TCP4U_CANCELLED : TCP4U_ERROR;
  }
  Skt4uRcd (ConSock, STATE_SERVER);

  Tcp4uLog (LOG4U_CALL, "setsockopt SOL_LINGER on socket %d", ConSock);
  setsockopt (ConSock, SOL_SOCKET, SO_LINGER,(LPSTR) &sling, sizeof sling);
  /* validite des pointeurs */
  if (IsBadWritePtr (pCSock, sizeof *pCSock))
   {
     TcpClose (& ConSock);
     Tcp4uLog (LOG4U_ERROR, "TcpAccept. invalid pointer");
     return TCP4U_BUFFERFREED;
   }
  else
   {
      *pCSock = ConSock;
      Tcp4uLog (LOG4U_EXIT, "TcpAccept");
      return TCP4U_SUCCESS;
   }
} /* TcpAccept */



/* ------------------------------------------------------------ */
/* TcpClose - Fermeture d'une socket                            */
/*            La socket pointie par pS est fermie, *pS est      */
/*            mise ` INVALID_SOCKET.                            */
/*          - Retourne 1 en cas de succhs, -1 sinon (en giniral */
/*            du ` l'erreur WSAEINTR)                           */
/*            constantes (TCP4U_SUCCESS et TCP4U_ERROR)               */
/* ------------------------------------------------------------ */
int API4U TcpClose (SOCKET far *pS)
{
SOCKET s;
struct timeval     TO;         /* Time Out structure    */
struct S_HistoSocket *pStat;   /* stats about socket    */

  Tcp4uLog (LOG4U_PROC, "TcpClose socket %d", *pS);

  if (*pS==(unsigned) INVALID_SOCKET)  return TCP4U_SUCCESS; /* bien passe */
  s = *pS;
  pStat = Skt4uGetStats (s);
  if (pStat!=NULL  &&  pStat->nState==STATE_LISTEN)
    {
      /* Ici une petite tempo pour les winsockets qui n'apprecient */
      /* pas la succession trop rapide Accept/Close                */
     TO.tv_sec = 0;
     TO.tv_usec = 100000l;     /* microsecondes */
     select (s+1, NULL, NULL, NULL, & TO);
    } /* pause */

  /* Si on effectue un shutdown (s, 2), certaines winsocket */
  /* n'envoie plus la trame de longueur 0 caracteristique   */
  /* de la fin de connexion.                                */
  /* On le remplace donc par le TcpFlush                    */
  if (pStat!=NULL  &&  pStat->nState!=STATE_LISTEN)  TcpFlush (s);

  Tcp4uLog (LOG4U_CALL, "closesocket socket %d", s);
  if (CloseSocket (s)==0)
    {
       Skt4uUnRcd (s);
       if (!IsBadWritePtr(pS, sizeof *pS))  *pS =(unsigned) INVALID_SOCKET;
       else 			      				Tcp4uLog (LOG4U_ERROR, "TcpClose: Invalid pointer");

       Tcp4uLog (LOG4U_EXIT, "TcpClose");
       return TCP4U_SUCCESS;
    }
  Tcp4uLog (LOG4U_ERROR, "closesocket socket %d", s);

return TCP4U_ERROR;
} /* TcpClose */



/* --------------------------------------------------------------------- */
/* TcpConnect : Tentative d'etablissemnt d'une connexion TCP             */
/*              Retourne TCP4U_ERROR, TCP4U_SUCCESS, TCP4U_HOSTUNKNONW   */
/* --------------------------------------------------------------------- */
int API4U TcpConnect (SOCKET far *pS, LPCSTR szHost,
                      LPCSTR szService, unsigned short far *lpPort)
{
int                   Rc;
struct sockaddr_in    saSockAddr;
struct servent far *  lpServEnt;
SOCKET                connect_skt;
unsigned short        Zero = 0;

   Tcp4uLog (LOG4U_PROC, "TcpConnect. Host %s, service %s, port %u", 
             szHost, 
             szService==NULL ? "none" : szService, 
             lpPort==NULL ? -1 : *lpPort);

  *pS = INVALID_SOCKET;  /* par defaut erreur */
  if (lpPort==NULL)  lpPort = & Zero;  /* evite de tester sans arret */
  /* --- 1er champ de saSockAddr : Port */
  if (szService==NULL)  lpServEnt =  NULL ;
  else
  {
     Tcp4uLog (LOG4U_DBCALL, "getservbyname %s/tcp", szService);
     lpServEnt = getservbyname (szService, "tcp") ;
  }
  saSockAddr.sin_port = lpServEnt!=NULL ?   lpServEnt->s_port : htons(*lpPort);
  if (saSockAddr.sin_port == 0)  return TCP4U_BADPORT;  /* erreur dans port */
  /* --- 2eme champ de saSockAddr : Addresse serveur */
  saSockAddr.sin_addr = Tcp4uGetIPAddr (szHost);
  if (saSockAddr.sin_addr.s_addr==INADDR_NONE)   return TCP4U_HOSTUNKNOWN;
  /* --- Dernier champ : liaison connectie */
  saSockAddr.sin_family      = AF_INET; /* on utilise le mode connecte TCP */
  /* --- creation de la socket */
  Tcp4uLog (LOG4U_CALL, "socket PF_INET, SOCK_STREAM");
  if ( (connect_skt = socket (PF_INET, SOCK_STREAM, 0))==SOCKET_ERROR)
  {
       Tcp4uLog (LOG4U_ERROR, "socket");
       return  TCP4U_NOMORESOCKET;
  }
  /* --- connect retourne INVALID_SOCKET ou numero valide */
  Tcp4uLog (LOG4U_CALL, "connect on host %s", inet_ntoa (saSockAddr.sin_addr));
  Rc = connect (connect_skt,(struct sockaddr far *) & saSockAddr, sizeof saSockAddr);
  /* --- enregistrement dans notre table */
  if (Rc==SOCKET_ERROR)
  {
      Tcp4uLog (LOG4U_ERROR, "connect");
      CloseSocket (connect_skt);  /* release buffer */
  }
  else                   Skt4uRcd (connect_skt, STATE_CLIENT);
 *lpPort = htons (saSockAddr.sin_port);
  if (IsBadWritePtr (pS, sizeof *pS))  
  {
       Tcp4uLog (LOG4U_ERROR, "TcpConnect: invalid pointer");
       return TCP4U_BUFFERFREED;
   }
  else *pS = Rc==SOCKET_ERROR ? INVALID_SOCKET : connect_skt;
  
  Tcp4uLog (LOG4U_EXIT, "TcpConnect");
return Rc==SOCKET_ERROR ?   TCP4U_CONNECTFAILED :  TCP4U_SUCCESS;
}  /* TcpConnect */



/* ------------------------------------------------------------ */
/* TcpFlush:  Vide le buffer relatif a une socket               */
/*    Retourne TCP4U_ERROR, TCP4U_SUCCESS, TCP4U_CLOSEDSOCKET   */
/* ------------------------------------------------------------ */
int API4U TcpFlush (SOCKET s)
{
int             nBR=-1;
char            szBuf[64];

  Tcp4uLog (LOG4U_PROC, "TcpFlush on socket %d", s);

  if (s==INVALID_SOCKET)  return TCP4U_ERROR;
  while ( TcpIsDataAvail (s)  &&  (nBR=recv (s,szBuf,sizeof szBuf,0)) > 0 )
      Tcp4uDump (szBuf, nBR, "Flushed");
  while ( TcpIsOOBDataAvail (s) &&(nBR=recv (s,szBuf,sizeof szBuf,MSG_OOB))>0 )
	  Tcp4uDump (szBuf, nBR, "Flushed");

   Tcp4uLog (LOG4U_EXIT, "TcpFlush");
if (nBR<0)    return IsCancelled() ? TCP4U_CANCELLED : TCP4U_ERROR;
if (nBR==0)   return TCP4U_SOCKETCLOSED;
return TCP4U_SUCCESS;
} /* TcpFlush */



/* ------------------------------------------------------------ */
/* TcpGetListenSocket                                           */
/*              Retourne TCP4U_ERROR, TCP4U_SUCCESS             */
/* ------------------------------------------------------------ */
int API4U TcpGetListenSocket (SOCKET far *pS, LPCSTR szService,
                            unsigned short far *lpPort, int nPendingConnection)
{
struct sockaddr_in   saSockAddr; /* specifications pour le Accept */
int                  nAddrLen;
SOCKET               ListenSock;
int                  One=1;
int                  Rc;
unsigned short       Zero = 0;
struct servent far * lpServEnt;

   Tcp4uLog (LOG4U_PROC, "TcpGetListenSocket on service %s, port %u", 
             szService==NULL ? "none" : szService, 
             lpPort==NULL ? 0 : *lpPort);

  *pS = INVALID_SOCKET;
   if (lpPort==NULL)  lpPort = & Zero;
   /* --- 1er champ de saSockAddr : Port */
  if (szService==NULL)  lpServEnt =  NULL ;
  else
  {
     Tcp4uLog (LOG4U_DBCALL, "getservbyname %s/tcp", szService);
     lpServEnt = getservbyname (szService, "tcp") ;
  }
   saSockAddr.sin_port = (lpServEnt!=NULL) ? lpServEnt->s_port : htons(*lpPort);
#ifdef CONTROL_EN_TROP
  if (saSockAddr.sin_port==0) return TCP4U_BADPORT; /* erreur attribution Port */
#endif  
  /* create socket */
  Tcp4uLog (LOG4U_CALL, "socket AF_INET, SOCK_STREAM");
  ListenSock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (ListenSock==INVALID_SOCKET)
  {
      Tcp4uLog (LOG4U_ERROR, "socket");
      return TCP4U_NOMORESOCKET;
  }
  saSockAddr.sin_family = AF_INET;
  saSockAddr.sin_addr.s_addr=INADDR_ANY;
  /* set options ReuseAddress and Linger */
  Tcp4uLog (LOG4U_CALL, "setsockopt SO_REUSEADDR on sock %d", ListenSock);
  setsockopt (ListenSock, SOL_SOCKET, SO_REUSEADDR, (char far *)&One, sizeof One);
  Tcp4uLog (LOG4U_CALL, "setsockopt SO_LINGER on sock %d", ListenSock);
  setsockopt (ListenSock, SOL_SOCKET, SO_LINGER,(char far *)&One, sizeof One);
  /* Bind name to socket */
  Tcp4uLog (LOG4U_CALL, "bind socket %d to %s", ListenSock, inet_ntoa (saSockAddr.sin_addr));
  Rc =   bind (ListenSock,(struct sockaddr far *) & saSockAddr, sizeof(struct sockaddr));
  if (Rc != SOCKET_ERROR)
  {
      Tcp4uLog (LOG4U_CALL, "listen on socket %d. %d pendind connections", ListenSock, nPendingConnection);
      Rc = listen (ListenSock, nPendingConnection);
  }
  if (Rc==SOCKET_ERROR)
  {
      Tcp4uLog (LOG4U_ERROR, "bind or listen on socket %d", WSAGetLastError (), ListenSock);
      CloseSocket (ListenSock);
      return  TCP4U_BINDERROR;
   }
  /* Get port id with getsockname */
  nAddrLen = sizeof saSockAddr;
  memset (& saSockAddr, 0, sizeof(struct sockaddr) );
  Tcp4uLog (LOG4U_DBCALL, "getsockname on socket %d", ListenSock);
  getsockname (ListenSock, (struct sockaddr far *) &saSockAddr, &nAddrLen);
 *lpPort = htons (saSockAddr.sin_port);
  Skt4uRcd (ListenSock, STATE_LISTEN);
  if (IsBadWritePtr (pS, sizeof *pS))  
  {
	  Tcp4uLog (LOG4U_ERROR, "TcpGetListenSocket: invalid pointer");
  	  return TCP4U_BUFFERFREED;
  }
  else                                *pS = ListenSock;

  Tcp4uLog (LOG4U_EXIT, "TcpGetListenSocket");
return TCP4U_SUCCESS;
}  /* TcpGetListenSock */



/* ------------------------------------------------------------ */
/* TcpRecv - Lecture d'une trame avec Timeout                   */
/*           TcpRecv recoit uBufSize octets de donnies          */
/*           la fonction s'arrjte avant si la socket est fermie */
/*           ou si une timeout arrive.                          */
/*         - Retourne le nombre d'octets lus,                   */
/*           TCP4U_SOCKETCLOSED si la socket a iti fermie       */
/*           TCP4U_ERROR sur une erreur de lecture              */
/*           TCP4U_TIMEOUT sur un TO                            */
/*           TCP4U_BUFFERFREED si libiration des buffers        */
/*         - Remarque : Pour iviter que le buffer appelant soit */
/*           libere de manihre asynchrone, la fonction utilise  */
/*           ses propres buffers.                               */
/*         - Ajout des modes  TCP4U_WAITFOREVER et TCP4U_WAIT   */
/* ------------------------------------------------------------ */

/* forme avec log */
int API4U TcpRecv (SOCKET s, LPSTR szBuf, unsigned uBufSize,
                    unsigned uTimeOut, HFILE hLogFile)
{
int Rc;
  Tcp4uLog (LOG4U_PROC, "TcpRecv on socket %d. Timeout %d, bufsize %d", s, uTimeOut, uBufSize);
  Rc = InternalTcpRecv (s, szBuf, uBufSize, uTimeOut, hLogFile);
  if (Rc>=TCP4U_SUCCESS)  Tcp4uDump (szBuf, Rc, DUMP4U_RCVD);
  Tcp4uLog (LOG4U_EXIT, "TcpRecv");
return Rc;
}  /* TcpRecv */


/* forme sans log */
int InternalTcpRecv (SOCKET s, LPSTR szBuf, unsigned uBufSize, 
                     unsigned uTimeOut, HFILE hLogFile)
{
char            cBuf;
LPSTR           p = NULL;
int             Rc, nUpRc;  /* Return Code of select and recv */
struct timeval  TO;         /* Time Out structure             */
struct timeval *pTO;        /* Time Out structure             */
fd_set          ReadMask;   /* select mask                    */


  if (s==INVALID_SOCKET)  return TCP4U_ERROR;
  FD_ZERO (& ReadMask);     /* mise a zero du masque */
  FD_SET (s, & ReadMask);   /* Attente d'evenement en lecture */

  /* detail des modes */
  switch (uTimeOut)
  {
      case  TCP4U_WAITFOREVER : pTO = NULL; 
                                break;
      case  TCP4U_DONTWAIT    : TO.tv_sec = TO.tv_usec=0 ; 
                                pTO = & TO;
                                break;
      /* Otherwise  uTimeout is really the Timeout */
      default :                 TO.tv_sec = (long) uTimeOut;
                                TO.tv_usec=0;
                                pTO = & TO;
                                break;
  }
  /* s+1 normally unused but better for a lot of bugged TCP Stacks */
  Tcp4uLog (LOG4U_CALL, "select on socket %d", s);
  Rc = select (s+1, & ReadMask, NULL, NULL, pTO);
  if (Rc<0) 
  {
     Tcp4uLog (LOG4U_ERROR, "select on socket %d", s);
     return  IsCancelled() ? TCP4U_CANCELLED : TCP4U_ERROR;
  }
  if (Rc==0)
  {
     Tcp4uLog (LOG4U_ERROR, "select on socket %d: Timeout", s);
     return  TCP4U_TIMEOUT;  /* timeout en reception           */
  }
  if (szBuf==NULL  ||  uBufSize==0)  
  {
    Tcp4uLog (LOG4U_EXIT, "TcpRecv");
	return TCP4U_SUCCESS;
  }
  if (uBufSize==1)         /* cas frequent : lecture caractere par caractere */
       p = & cBuf;
  else
  {
       p = Calloc (uBufSize, 1);
       if (p==NULL)  return TCP4U_INSMEMORY;
  }
  
  Tcp4uLog (LOG4U_CALL, "recv socket %d, buffer %d bytes", s, uBufSize);
  Rc = recv (s, p, uBufSize, 0);  /* chgt 11/01/95 */
 
  switch (Rc)
  {
       case SOCKET_ERROR : 
              Tcp4uLog (LOG4U_ERROR, "recv socket %d", s);
              nUpRc = IsCancelled() ? TCP4U_CANCELLED : TCP4U_ERROR ; 
              break;
       case 0            : 
              Tcp4uLog (LOG4U_ERROR,"socket %d: Host has closed connection", s);
              nUpRc = TCP4U_SOCKETCLOSED ; 
              break;
       default :
              if (hLogFile!=HFILE_ERROR)          Write (hLogFile, p, Rc);
              if  (IsBadWritePtr (szBuf, 1))      nUpRc = TCP4U_BUFFERFREED;
              else                                memcpy (szBuf, p, nUpRc=Rc);
              break;
  } /* translation des codes d'erreurs */
  if (p!=NULL  && uBufSize!=1)        Free (p);
return nUpRc;
} /* TcpRecv */



/* ------------------------------------------------------------ */
/* TcpSend :  Envoi de uBufSize octets vers le distant          */
/*            Retourne TCP4U_ERROR ou TCP4U_SUCCESS                   */
/* ------------------------------------------------------------ */

/* forme avec log */
int API4U TcpSend (SOCKET s, LPCSTR szBuf, unsigned uBufSize,
                   BOOL bHighPriority, HFILE hLogFile)
{
int      Rc;
  Tcp4uLog (LOG4U_PROC,"TcpSend on socket %d. %d bytes to be sent",s,uBufSize);
  Rc = InternalTcpSend (s, szBuf, uBufSize, bHighPriority, hLogFile);
  if (Rc==TCP4U_SUCCESS)  Tcp4uDump (szBuf, uBufSize, DUMP4U_SENT);
  Tcp4uLog (LOG4U_EXIT, "TcpSend");
return Rc;
}  /* TcpSend */

/* forme sans log */
int InternalTcpSend (SOCKET s, LPCSTR szBuf, unsigned uBufSize,
                     BOOL bHighPriority, HFILE hLogFile)
{
int      Rc;
unsigned Total;
LPSTR    p;

  if (s==INVALID_SOCKET)  return TCP4U_ERROR;
  p = Calloc (uBufSize, 1);
  if (p==NULL)  return TCP4U_INSMEMORY;
  memcpy (p, szBuf, uBufSize);
  if (hLogFile!=HFILE_ERROR)    Write (hLogFile, p, uBufSize);

  for ( Total = 0, Rc = 1 ;  Total < uBufSize  &&  Rc > 0 ;  Total += Rc)
    {
      Tcp4uLog (LOG4U_CALL, "send on socket %d. %d bytes to be sent", 
                s, uBufSize-Total);
      Rc = send (s, & p[Total], uBufSize-Total, bHighPriority ? MSG_OOB : 0);
      if (Rc<=0)   Tcp4uLog (LOG4U_ERROR, "send on socket %d.", s);
    }
  Free(p);
  
return Total>=uBufSize ? TCP4U_SUCCESS :  IsCancelled() ? TCP4U_CANCELLED : TCP4U_ERROR;
} /* TcpSend */




/* ******************************************************************* */
/*                                                                     */
/* Partie I (suite) :  Fonctions d'information                         */
/*                                                                     */
/*    TcpGetLocalID                                                    */
/*    TcpGetRemoteID                                                   */
/*    TcpIsDataAvail                                                   */
/*                                                                     */
/* ******************************************************************* */



/* ------------------------------------------------------------ */
/* TcpGetLocalID:   Retourne nom et adresse IP de la station    */
/*                  Retourne TCP4U_SUCCESS, TCP4U_ERROR         */
/* ------------------------------------------------------------ */
int API4U TcpGetLocalID (LPSTR szStrName, int uNameSize, DWORD far *lpAddress)
{
char szName[128];
struct hostent far *lpHostEnt;
int    Rc;

  Tcp4uLog (LOG4U_PROC, "TcpGetLocalID");

   if (lpAddress!=NULL)  memset (lpAddress, 0, sizeof (DWORD));
   /* on envisage 2 methodes pour obtenir le nom du PC */
   /* d'abord un appel a gethostname                   */
  Tcp4uLog (LOG4U_DBCALL, "gethostname");
   if (gethostname (szName, sizeof szName)==SOCKET_ERROR)  return TCP4U_ERROR;
   if (lpAddress != NULL)
     {
        Tcp4uLog (LOG4U_DBCALL, "gethostbyname on host %s", szName);
        lpHostEnt = gethostbyname (szName);
        if (lpHostEnt==NULL) return TCP4U_ERROR;
        memcpy (lpAddress, lpHostEnt->h_addr_list[0], lpHostEnt->h_length);
     }
   if (szStrName!=NULL)
     {
       Strcpyn (szStrName, szName, uNameSize);
       Rc =  Strlen(szName)>Strlen(szStrName) ? TCP4U_OVERFLOW : TCP4U_SUCCESS;
     }
   else  Rc = TCP4U_SUCCESS;

   Tcp4uLog (LOG4U_EXIT, "TcpGetLocalID");
return TCP4U_SUCCESS;   
} /* TcpGetLocalID */



/* ------------------------------------------------------------ */
/* TcpGetRemoteID:  Retourne nom et adresse IP du distant       */
/*                  Retourne TCP4U_SUCCESS, TCP4U_ERROR               */
/* ------------------------------------------------------------ */
int API4U TcpGetRemoteID (SOCKET s, LPSTR szStrName, int uNameSize, 
              DWORD far *lpAddress)
{
struct sockaddr_in  SockAddr;
int                 SockAddrLen=sizeof SockAddr;
struct hostent far *lpHostEnt;

  Tcp4uLog (LOG4U_PROC, "TcpGetRemoteID on socket %d", s);

   /* determiner l'adress IP de la station distante */
   Tcp4uLog (LOG4U_DBCALL, "getpeername on socket %d", s);
   if (getpeername (s, (struct sockaddr far *) & SockAddr, & SockAddrLen)==SOCKET_ERROR) 
   {
        Tcp4uLog (LOG4U_ERROR, "getpeername on socket %d", s);
        return TCP4U_ERROR;
   }
   if (lpAddress!=NULL) 
      memcpy (lpAddress, & SockAddr.sin_addr.s_addr,
          sizeof SockAddr.sin_addr.s_addr);
   if (szStrName!=NULL  &&  uNameSize > 0)  
   {
     /* determiner par gethostbyaddr le nom de la station */
     szStrName[0]=0;  /* si erreur */
     Tcp4uLog (LOG4U_DBCALL, "gethostbyaddr on host %s", inet_ntoa (SockAddr.sin_addr));
     lpHostEnt = gethostbyaddr ((LPSTR) & SockAddr.sin_addr.s_addr, 4, PF_INET);
     if (lpHostEnt!=NULL)  Strcpyn (szStrName, lpHostEnt->h_name, uNameSize);
     else                  Tcp4uLog (LOG4U_ERROR, "gethostbyaddr");
   }

  Tcp4uLog (LOG4U_EXIT, "TcpGetRemoteID");
return TCP4U_SUCCESS;
} /* TcpGetRemoteID */



/* ------------------------------------------------------------- */
/* TcpIsDataAvail: Retourne VRAI si des donnees sont disponibles */
/* ------------------------------------------------------------- */
BOOL API4U TcpIsDataAvail (SOCKET s)
{
unsigned long  ulData;
int            Rc;
  Tcp4uLog (LOG4U_PROC, "TcpIsDataAvail on socket %d", s);

  Tcp4uLog (LOG4U_CALL, "Ioctl FIONREAD on socket %d", s);
  Rc = IoctlSocket (s, FIONREAD, & ulData);
  Tcp4uLog (LOG4U_EXIT, "TcpIsDataAvail");
return Rc==0  &&  ulData>0;
} /* TcpIsDataAvail */


/* ------------------------------------------------------------- */
/* TcpIsOOBDataAvail: Retourne VRAI si des donnees Out Of Band   */
/*                    sont disponibles                           */
/* ------------------------------------------------------------- */
BOOL API4U TcpIsOOBDataAvail (SOCKET s)
{
unsigned long  ulData;
int            Rc;

  Tcp4uLog (LOG4U_PROC, "TcpIsOOBDataAvail on socket %d", s);

  Rc = IoctlSocket (s, SIOCATMARK, & ulData);
  Tcp4uLog (LOG4U_EXIT, "TcpIsOOBDataAvail");
return Rc==0  &&  ! ulData;
} /* TcpIsOOBDataAvail */



/* ******************************************************************* */
/*                                                                     */
/* Partie II :  Gestion d'un mini- protocole                           */
/*                                                                     */
/*                                                                     */
/*              TcpPPSend  -  TcpPPRecv                                */
/*                                                                     */
/*                                                                     */
/* ******************************************************************* */



/* ------------------------------------------------------------ */
/* TcpPPRecv - Lecture d'une trame avec Timeout                 */
/*           TcpPPRecv recoit uBufSize octets de donnies        */
/*           la fonction s'arrjte avant si la socket est fermie */
/*           ou si un timeout arrive.                           */
/*         - Retourne le nombre d'octets lus,                   */
/*           TCP4U_SOCKETCLOSED si la socket a iti fermie          */
/*           TCP4U_ERROR sur une erreur de lecture                 */
/*           TCP4U_TIMEOUT sur un TO                               */
/*           TCP4U_BUFFERFREED si libiration des buffers           */
/*         - Remarque : Pour iviter que le buffer appelant soit */
/*           libiri de manihre asynchrone, la fonction utilise  */
/*           ses propres buffers.                               */
/* ------------------------------------------------------------ */
int API4U TcpPPRecv (SOCKET s, LPSTR szBuf, unsigned uBufSize,
                     unsigned uTimeOut, BOOL bExact, HFILE hLogFile)
{
LPSTR           p, q;
int             Rc, nUpRc;  /* Return Code of select and recv */
unsigned short  nToBeReceived, nReceived;

  Tcp4uLog (LOG4U_PROC, "TcpPPRecv on socket %d. Timeout %d, buffer %d bytes", s, uTimeOut, uBufSize);

  if (s==INVALID_SOCKET)  return TCP4U_ERROR;
  Rc = TcpRecv (s, (LPSTR) & nToBeReceived, 2, uTimeOut, hLogFile);
  if (Rc<TCP4U_SUCCESS)  return Rc;

  /* remise dans l'ordre PC */
  nToBeReceived = ntohs (nToBeReceived);
  /* Si longueur connue a l'avance, sinon ne pas depasser le buffer donne */
  if (bExact && nToBeReceived!=uBufSize)  return TCP4U_UNMATCHEDLENGTH;
  if (uBufSize < nToBeReceived)           return TCP4U_OVERFLOW;
  if (nToBeReceived==0)                   return TCP4U_EMPTYBUFFER;

  /* On elimine une des causes de crash: on lit dans un buffer qui n'existe plus */
  q = p = Calloc (nToBeReceived, 1);
  if (p==NULL)  return TCP4U_INSMEMORY;

  nReceived = 0;
  nUpRc=0;
  do   /* On boucle tant qu'on a pas eu les nToBeReceived octets */
  {
      nUpRc = TcpRecv (s, q, nToBeReceived - nReceived, uTimeOut/4 + 1, hLogFile);
      nReceived += (short) nUpRc;
      q += nUpRc;
    }
  while (nUpRc>0  &&  nReceived<nToBeReceived);
  /* Analyse du code de retour */
  if (nUpRc >= TCP4U_SUCCESS)
  {
    if  (IsBadWritePtr (szBuf, nToBeReceived))  nUpRc =  TCP4U_BUFFERFREED;
    else                                        memcpy (szBuf, p, nUpRc=nToBeReceived);
  }
  Free (p);

  Tcp4uLog (LOG4U_EXIT, "TcpPPRecv");
return nUpRc;
} /* TcpPPRecv */



/* -------------------------------------------------------------- */
/* TcpPPSend :  Envoi de uBufSize octets vers le distant          */
/*              Retourne TCP4U_ERROR ou TCP4U_SUCCESS                   */
/* -------------------------------------------------------------- */
int API4U TcpPPSend (SOCKET s, LPCSTR szBuf, unsigned uBufSize, HFILE hLogFile)
{
int      Rc;
unsigned short Total;

  Tcp4uLog (LOG4U_PROC, "TcpPPSend on socket %d. %d bytes to send", s, uBufSize);

  if (s==INVALID_SOCKET)  return TCP4U_ERROR;
  /* Envoi de la longueur de la trame sur 2 octets */
  Total = htons ((unsigned short) uBufSize);
  Rc = TcpSend (s, (void far *) & Total, 2, FALSE, hLogFile);

  /* cas d'une sequence vide -> envoi de 0 OK: retour OK */
  if (Rc>=TCP4U_SUCCESS && uBufSize>0)
     /* maintenant on envoie la sequence (qui n'est pas vide) */
     Rc = TcpSend (s, szBuf, uBufSize, FALSE, hLogFile);

  Tcp4uLog (LOG4U_EXIT, "TcpPPSend");
return Rc;
} /* TcpPPSend */




/* ******************************************************************* */
/*                                                                     */
/* Partie III :  Lecture Jusqu'a ...                                   */
/*                                                                     */
/*                                                                     */
/* ******************************************************************* */



/* ------------------------------------------------------------ */
/* TcpRecvUntilStr                                              */
/*           Recois des donnees jusqu'a                         */
/*              - Une erreur                                    */
/*              - Une Fermeture de socket                       */
/*              - un timeout                                    */
/*              - la reception de la chaine szStop              */
/*              - le remplissage du buffer                      */
/* Le Retour soit un code d'erreur, soit TCP4U_SUCCESS             */
/* Si c'est TCP4U_OVERFLOW, uBufSize octes ont iti lus, mais       */
/* la chaine szStop n'a pas iti trouvie                         */
/* Si c'est TCP4U_SOCKETCLOSED, le distant a ferme la connexion    */
/* La chaine szStop n'est pas ajoutie au buffer                 */
/* lpBufSize contient le nombre d'octets lus                    */
/* NOTE: Cette fonction est incompatible avec La pile de Novell */
/*       LAN Workplace                                          */
/* ------------------------------------------------------------ */

/* redeclaration de tolower: MSCV2 semble avoir des problemes avec */
static char ToLower (char c)
{
return (c>='A'  &&  c<='Z') ?  c + ('a' -'A') : c;
} /* ToLower */

/* compare les chaines s1 et s2 sur nLength caracteres */
/* retourne TRUE si les deux chaines sont identiques   */
static BOOL uMemcmp (LPSTR s1, LPSTR s2, int nLength, BOOL bCaseSensitive)
{
int Evan;
  if (bCaseSensitive)
       return memcmp (s1, s2, nLength)==0;
  else
    {
       for (Evan=0 ; Evan<nLength  &&  ToLower(s1[Evan])==ToLower(s2[Evan]) ;  Evan++);
       return Evan==nLength;
    }
} /* uMemcmp */

/* Sous procedure : Recherche de la sous chaine s2 dans s1   */
/* Renvoie          l'index de s1 auquel on peut trouver s2  */
/*              ou  -1 si s2 n'est pas incluse dans s1, dans */
/*                  ce cas, *nProx est le nombre d'octets de */
/*                  s2 contenu ` la fin de s1                */
static int FindPos (LPSTR s1, int ns1, LPSTR s2, int ns2, int *npProx, BOOL bCaseSensitive)
{
int Ark;
BOOL bNotFound;


   if (*npProx!=0)  /* on compare aussi si l'inclusion ne continue pas */
    {
      if (ns1<ns2-*npProx   &&  uMemcmp (s1, & s2[*npProx], ns1, bCaseSensitive))
            { *npProx+=ns1 ; return -1; }
      if (ns1>=ns2-*npProx  &&  uMemcmp (s1, & s2[*npProx], ns2-*npProx, bCaseSensitive))
            { return 0;  }
    } /* traitement des antecedents */

   for ( Ark=0, bNotFound=TRUE;
         Ark<=ns1-ns2  &&  (bNotFound= ! uMemcmp (& s1[Ark], s2, ns2, bCaseSensitive)) ;
         Ark++) ;   /* recherche de s2 */
   if (bNotFound)
    {
       for ( *npProx = min (ns1, ns2-1) ;
             *npProx>0 && !uMemcmp (& s1[ns1-*npProx], s2, *npProx, bCaseSensitive);
            (*npProx)-- );
    }
return bNotFound ? -1 : Ark;  /* si bNotFounf, *npProx est positionne */
} /* FindPos */

int API4U TcpRecvUntilStr (SOCKET s, LPSTR szBuf,unsigned far *lpBufSize,
                           LPSTR szStop, unsigned uStopSize, BOOL bCaseSensitive,
                           unsigned uTimeOut, HFILE hLogFile)
{
int Rc;
  Tcp4uLog ( LOG4U_PROC, 
            "TcpRecvUntilStr on socket %d. Timeout %d, buffer %d bytes", 
             s, uTimeOut, *lpBufSize);
  Rc = InternalTcpRecvUntilStr (s, szBuf, lpBufSize, szStop, uStopSize, 
                                bCaseSensitive, uTimeOut, hLogFile);
  if (Rc==TCP4U_SUCCESS)    Tcp4uDump (szBuf, *lpBufSize, DUMP4U_RCVD);
  if (Rc==TCP4U_OVERFLOW)   Tcp4uDump (szBuf, *lpBufSize, "Overflow");
  Tcp4uLog (LOG4U_EXIT, "TcpRecvUntilStr");
return Rc;
} /* TcpRecvUntilStr */


int InternalTcpRecvUntilStr (SOCKET s, LPSTR szBuf,unsigned far *lpBufSize,
                             LPSTR szStop, unsigned uStopSize, BOOL bCaseSensitive,
                             unsigned uTimeOut, HFILE hLogFile)
{
unsigned        uNbRecus;
static int      nProxy; /* Proxy sert si clef partagee sur 2 trames */
LPSTR           pTest = NULL;
int             Rc;
int             Idx;

#define XX_RETURN(x,s)             \
{  if (pTest!=NULL)   Free(pTest); \
   *lpBufSize=uNbRecus;            \
   Tcp4uLog (LOG4U_ERROR, IsCancelled() ? "call cancelled" : s);      \
   return IsCancelled() ? TCP4U_CANCELLED : x;   }

  if (s==INVALID_SOCKET || *lpBufSize==0)  return TCP4U_ERROR;
  /* On prend un buffer qui permet de lire la trame,   */
  /* la chaine de stop et qui soit toujours 0-terminee */
  /* Le recv suivant est sur de ne pas bloquer, vu que */
  /* le PEEK a rendu OK                                */
  pTest = Calloc (*lpBufSize+uStopSize+1, 1);
  if (pTest==NULL)  return TCP4U_INSMEMORY;
  uNbRecus = 0;
  do
   {
      Rc = InternalTcpRecv (s, NULL, 0,  uTimeOut, HFILE_ERROR);
      /* ereur select */
      if (Rc != TCP4U_SUCCESS)   XX_RETURN (Rc, "Timeout");

      /* Lecture en MSG_PEEK */
      Tcp4uLog (LOG4U_CALL, "recv on socket %d. MSG_PEEK, buffer %d bytes", 
                s, *lpBufSize + uStopSize - uNbRecus);
      Rc = recv (s, & pTest[uNbRecus], *lpBufSize+uStopSize-uNbRecus, MSG_PEEK);
      if (Rc<=0)                XX_RETURN (Rc, "TcpRecvUntilStr");

      /* --- Maintenant on distingue 2 cas:  */
      /* Cas 1 : la clef est dans le buffer, */
      Idx = FindPos (& pTest[uNbRecus], Rc, szStop, uStopSize, 
                     & nProxy, bCaseSensitive);
      if (Idx!=-1)  /* la clef est disponible -> pas de probleme */
      {
         if ( IsBadWritePtr (& szBuf[uNbRecus], Idx) )
             XX_RETURN (TCP4U_BUFFERFREED, "TcpRecvUntilStr: invalid pointer")
         else
         {
            if (Idx!=0)
            {
                InternalTcpRecv (s, & szBuf[uNbRecus], Idx, 0, hLogFile);
                uNbRecus += Idx;
             }
             else  uNbRecus -= nProxy;
            *lpBufSize = uNbRecus;
             InternalTcpRecv (s, pTest, uStopSize - (Idx==0 ? nProxy:0), 
                              0, hLogFile);
             nProxy = 0; /* pour la prochaine recherche */
             Free (pTest);
             return TCP4U_SUCCESS;
           }
        } /* chaine trouvee */
      else
      /* Cas 2 : la clef n'est pas dans le buffer */
        {int nToBeReceived = min (Rc, (signed) (*lpBufSize-uNbRecus));
          if (IsBadWritePtr (& szBuf[uNbRecus], nToBeReceived) )
          { 
              Free (pTest);
              Tcp4uLog (LOG4U_ERROR, "TcpRecvUntilStr: invalid pointer");
              return TCP4U_BUFFERFREED; 
          }
          else
          {
             InternalTcpRecv  (s, & szBuf[uNbRecus], nToBeReceived , 0, hLogFile);
             uNbRecus += nToBeReceived;
          }
        } /* chaine pas trouvee */
    }
  while (uNbRecus<*lpBufSize);
  Free (pTest);
  *lpBufSize = uNbRecus;

#undef XX_RETURN
return TCP4U_OVERFLOW;
} /* InternalTcpRecvUntilStr */


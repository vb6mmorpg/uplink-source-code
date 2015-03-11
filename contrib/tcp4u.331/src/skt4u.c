/*
 * Tcp4u v 3.31         Last Revision 06/06/1997  3.20
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    skt4u.c
 * Purpose: Stats on socket usage
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




/* ****************************************************************** */
/* Etage 0 :                                                          */
/*           Gestion des donnees internes                             */
/*                                                                    */
/* ****************************************************************** */

static BOOL bInitDone=FALSE;  /* Init has been done           */


/* ------------------------------------------------------------------ */
/* Historiques des ouvertures / fermetures                            */
/* ------------------------------------------------------------------ */

static struct S_HistoSocket HistoSocket[512];

#define FindFirstFreeIdx()   FindSocketIdx (INVALID_SOCKET)
#define INVALID_INDEX        -1
#ifndef INVALID_SOCKET
#  define INVALID_SOCKET     (SOCKET) (-1)
#endif  /* INVALID_SOCKET */


/* ------------------------------------------------------------------ */
/* recherche d'un index dans le tableau                               */
/* ------------------------------------------------------------------ */
static int FindSocketIdx (SOCKET s)
{
int Ark;
   for (Ark=0 ; Ark<SizeOfTab(HistoSocket) && s!=HistoSocket[Ark].skt ; Ark++);
return  Ark<SizeOfTab(HistoSocket) ?  Ark : INVALID_INDEX;
}  /* FindSocketIdx */



/* ------------------------------------------------------------------ */
/* Enregistrement d'une socket dans l'historique                      */
/* ------------------------------------------------------------------ */
void Skt4uRcd (SOCKET sNewSkt, int nState)
{
int Ark;
  if ( (Ark=FindFirstFreeIdx()) != INVALID_INDEX) 
   {
      HistoSocket[Ark].skt    = sNewSkt;
      HistoSocket[Ark].nState = nState;
      HistoSocket[Ark].nRcv = HistoSocket[Ark].nSnd = 0;
      HistoSocket[Ark].hTask  = GetCurrentTask ();
   } 
} /* RcdSocket */
              
              

/* ------------------------------------------------------------------ */
/* Elimination d'une socket dans l'historique                         */
/* ------------------------------------------------------------------ */
void Skt4uUnRcd (SOCKET sClosedSkt)   
{
int Ark;
  if ( (Ark=FindSocketIdx(sClosedSkt)) != INVALID_INDEX) 
   {
      HistoSocket[Ark].skt = INVALID_SOCKET;
   } 
}/* UnRcdSocket */



/* ********************************************************************** */
/* Etage I :                                                              */
/*           Initialisations                                              */
/* ********************************************************************** */


/* ----------------------------------------------------- */
/* Initialisation                                        */
/* Pour Windows, on verifie que les sockets attribuees   */
/* correspondent a des taches actives.                   */
/* ----------------------------------------------------- */
int Skt4uInit (void)
{
int Ark;

   if (! bInitDone)  /* Aucune socket ouverte */
     {
       for (Ark=0 ; Ark<SizeOfTab(HistoSocket) ; Ark++)
          HistoSocket[Ark].skt = INVALID_SOCKET;
       bInitDone=TRUE;  
     }
#ifdef WINDOWS
   /* liberation des sockets appartenant a des taches mortes */
   else
    {
       for (Ark=0 ;  Ark<SizeOfTab(HistoSocket) ; Ark++)
     if (      HistoSocket[Ark].skt!=INVALID_SOCKET 
           && !IsTask(HistoSocket[Ark].hTask) )
            TcpClose (& HistoSocket[Ark].skt);
    }  /* InitDone */
#endif
return TCP4U_SUCCESS;
} /* Skt4wInit */



/* ------------------------------------------------------------------ */
/* Destructeurs                                                       */
/* ------------------------------------------------------------------ */
int Skt4uCleanup (void)
{
int Ark;

   if (bInitDone)
     {
       /* blocking call ? */
       if (WSAIsBlocking())  
        {
           WSACancelBlockingCall (); 
           return TCP4U_ERROR; 
         }
       else 
        {
          /* TcpClose (&s) force s a -1 -> pas besoin de UnrcdSkt */
          for (Ark=0 ; Ark<SizeOfTab(HistoSocket) ; Ark++)
             if (    HistoSocket[Ark].hTask == GetCurrentTask () 
                  && HistoSocket[Ark].skt   != INVALID_SOCKET )
                  TcpClose (& HistoSocket[Ark].skt);  
          WSACleanup ();
        }  /* requete non bloquante */ 
     } /* bInitDone */
return TCP4U_SUCCESS;
} /* Skt4wCleanup */



/* *************************************************************** */
/* Etage II :                                                      */
/*             Statistiques                                        */
/* *************************************************************** */

/* ------------------------------------------------------------------ */
/* Add the number of received bytes                                   */
/* ------------------------------------------------------------------ */
void Skt4uAddRcvSocket (SOCKET sSkt, int nRcv)
{
int Ark;
   if ( (Ark=FindSocketIdx(sSkt)) != INVALID_INDEX)
        HistoSocket[Ark].nRcv += nRcv;
} /* SktAddRcvSocket */


/* ------------------------------------------------------------------ */
/* Add the number of sent bytes                                       */
/* ------------------------------------------------------------------ */
void Skt4uAddSndSocket (SOCKET sSkt, int nSnd)
{
int Ark;
   if ( (Ark=FindSocketIdx(sSkt)) != INVALID_INDEX)
      HistoSocket[Ark].nSnd += nSnd;
} /* SktAddSndSocket */

/* ------------------------------------------------------------------ */
/* Returns a pointer on the struct for socket sSkt                    */
/* ------------------------------------------------------------------ */
struct S_HistoSocket *Skt4uGetStats (SOCKET sSkt)
{
int Ark;
   if ( (Ark=FindSocketIdx(sSkt)) != INVALID_INDEX)
     {
        return & HistoSocket[Ark];
     }
return NULL;
} /* S_HistoSocket */



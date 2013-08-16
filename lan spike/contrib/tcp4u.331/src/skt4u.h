/*
 * Tcp4u v 3.31
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    skt4u.h
 * Purpose: Internal header file. Lowest layer of the library
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



#ifndef _SKT4U_LOADDED
#define _SKT4U_LOADDED

     
/* ****************************************************************** */
/*                                                                    */
/* Etage 0 : Gestion des donnees internes                             */
/*                                                                    */
/* ****************************************************************** */


#define   STATE_LISTEN   12
#define   STATE_CLIENT   13
#define   STATE_SERVER   14
 
struct S_HistoSocket
{
  SOCKET skt;
  int    nState;
  long   nRcv;
  long   nSnd;
  HTASK  hTask;
} ; /* struct S_HistoSocket */

#define  TCP4U_SUCCESS           1  /* >=1 function OK            */
#define  TCP4U_ERROR            -1  /* error                      */
#define  TCP4U_TIMEOUT          -2  /* timeout has occured        */
#define  TCP4U_BUFFERFREED      -3  /* the buffer has been freed  */
#define  TCP4U_HOSTUNKNOWN      -4  /* connect to unknown host    */
#define  TCP4U_NOMORESOCKET     -5  /* all socket has been used   */
#define  TCP4U_NOMORERESOURCE   -5  /* or no more free resource   */
#define  TCP4U_CONNECTFAILED    -6  /* connect function has failed*/
#define  TCP4U_UNMATCHEDLENGTH  -7  /* TcpPPRecv : Error in length*/
#define  TCP4U_BINDERROR        -8  /* bind failed (Task already started?) */
#define  TCP4U_OVERFLOW         -9  /* Overflow during TcpPPRecv  */
#define  TCP4U_EMPTYBUFFER     -10  /* TcpPPRecv receives 0 byte  */
#define  TCP4U_CANCELLED       -11  /* Call cancelled by signal   */
#define  TCP4U_INSMEMORY       -12  /* Not enough memory          */
#define  TCP4U_SOCKETCLOSED      0  /* Host has closed connection */

/* ----------- Functions ---------- */
void Skt4uRcd (SOCKET sNewSkt, int nState);  /* new socket created    */
void Skt4uUnRcd (SOCKET sNewSkt);            /* socket destroyed      */
int  Skt4uCleanup (void);                    /* clean internal tables */
int  Skt4uInit (void);                       /* inits internal tables */
struct S_HistoSocket *Skt4uGetStats (SOCKET s); /* Get Histo data     */

#endif  /* _SKT4U_LOADDED */


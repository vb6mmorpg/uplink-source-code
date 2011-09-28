/*
 * Tcp4u v 3.31
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    dimens.h
 * Purpose: Internal header file
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


void Tcp4uLog (unsigned uLevel, LPCSTR szText, ...);


/* internal dimensions                  */
#define  HOST_LENGTH      256
#define  SERVICE_LENGTH   64
#define  FILE_LENGTH      512

/* les symboles des sump */
#define  DUMP4U_SENT    ">"
#define  DUMP4U_RCVD    "<"

/* internal functions                    */
/* these interface are subject to change */
int    Tcp4uAtoi (LPCSTR p);
long   Tcp4uAtol (LPCSTR p);
int    Tcp4uStrncasecmp (LPCSTR s1, LPCSTR s2, size_t n);
LPSTR  Tcp4uStrIStr (LPCSTR s1, LPCSTR s2);
struct in_addr Tcp4uGetIPAddr (LPCSTR szHost);

/* Tcp4u.c internal cals */
int InternalTcpSend (SOCKET s, LPCSTR szBuf, unsigned uBufSize,
                     BOOL bHighPriority, HFILE hLogFile);
int InternalTcpRecv (SOCKET s, LPSTR szBuf, unsigned uBufSize, 
                     unsigned uTimeOut, HFILE hLogFile);
int InternalTcpRecvUntilStr (SOCKET s, LPSTR szBuf,unsigned far *lpBufSize,
                             LPSTR szStop, unsigned uStopSize, BOOL bCaseSensitive,
                             unsigned uTimeOut, HFILE hLogFile);
/* tn4u.c internal cals */
int InternalTnReadLine (SOCKET s,LPSTR szBuf,UINT uBufSize,UINT uTimeOut,
                        HFILE hf);
/* Udp4u.c internal cals */
int InternalUdpRecv (LPUDPSOCK pUdp,  LPSTR sData, int nDataSize, 
                     unsigned uTimeOut, HFILE hLogFile);
int InternalUdpSend (LPUDPSOCK Udp,LPCSTR sData, int nDataSize,
                     BOOL   bHighPriority, HFILE hLogFile);


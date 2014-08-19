/*
 * Tcp4u v 3.31          Last Revision 27/02/1998  3.30
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    build.h
 * Purpose: Common header file. Group all includes needed 
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


#if defined (UNIX)
#      define  API4U
#elif defined (_WINDOWS)
#  ifdef _WIN32
#      define  API4U  PASCAL
#  else
#      define  API4U  _export PASCAL FAR
#  endif
#endif


/* --------------------------------------------------- */
/* The exported include files  should compile properly */
/* --------------------------------------------------- */
#include "tcp4u.h"
#include "udp4u.h"
#include "http4u.h"
#include "smtp4u.h"



/* --------------------------------------------------- */
/* The rest of them will have more troubles..........  */
/* --------------------------------------------------- */
#define HPUX_SOURCE

#include <memory.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WINDOWS
#  include <windows.h>
#  include <windowsx.h>
#  include <winsock.h>
#endif

#ifdef UNIX
#   include <stdio.h>
#   include <unistd.h>
#   include <errno.h>
#   include <sys/types.h>
#   include <sys/ioctl.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <netdb.h>
#ifndef HPUX
#   include <arpa/inet.h>
#endif
#   include <fcntl.h>
#   include <time.h>
/* systemes un peu speciaux --------------------------------------- */
#ifdef AIX
#  include <sys/select.h>
#endif
#if defined (SOLARIS) || defined (SunOS)
#  include <sys/sockio.h>
#endif
# include <sys/time.h>

/* ---------------------------------------------------------------- */
/* Et si ca ne marche toujours pas, on essaie les definitions :---- */
#ifndef INADDR_NONE
#   define INADDR_NONE -1
#endif
#ifndef FIONREAD
#   define FIONREAD    _IOR('f', 127, int)   /* get number of bytes to read */
#endif
#ifndef SIOCATMARK
#   define SIOCATMARK  _IOR('s',  7, int)    /* at oob mark? */
#endif
/* ---------------------------------------------------------------- */
#endif /* UNIX */


#include "port.h"         /* fonctions de portage */
#include "skt4u.h"        /* define internes      */
#include "dimens.h"       /* size of communly used data */


#ifndef SizeOfTab
#  define SizeOfTab(x)   (sizeof x / sizeof (*x))
#endif
#ifndef min
#  define min(a,b)       ( (a)<(b) ? (a) : (b) )
#endif


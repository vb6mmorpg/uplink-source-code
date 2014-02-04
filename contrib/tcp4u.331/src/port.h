/*
 * Tcp4u v 3.31
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    port.h
 * Purpose: Portability header file. Allow an uniq code
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



/*
 * Fichier d'equivalence Windows/Unix
 * ----------------------------------
 *
 * Cette serie de declarations definit si necessaire
 *          SOCKET  -> un descripteur de socket
 *          HTASK   -> un descripteur de process
 *          BOOL    -> le type VRAI/FAUX
 *          HFILE   -> un descripteur de fichier
 *          API4U   -> le type des APIs
 *          UINT    -> unsigned int
 *          DWORD   -> entier 4 octets
 *          far     -> signification uniquement pour Windows 16
 *          TRUE
 *          FALSE
 *          NULL
 *          CALLBACK 
 *          FARPROC
 *  les fonctions :
 *          GetCurrentTask -> rend un identifiant de tache
 *          WSAIsBlocking() -> rend VRAI si une fonction bloquante en cours
 *          WSACancelBlockingCall()
 *          WSAGetLastError()
 *          WSACleanup()
 *          WSAIsBadWritePtr()
 *          IsCancelled()  -> L'ordre a-t-il ete interrompu
 *          CloseSocket()
 *          IoctlSocket()  -> operation "de bas niveau"
 *          Write          -> ecriture dans un fichier HFILE
 *          Close          -> close et _hclose un fichier HFILE
 *          Open           -> open et _hopen  un fichier HFILE
 *          Strlen         -> fonction standard C, lstrlen en Windows
 *          Strcpy         -> fonction standard C, lstrcpy en Windows
 *          Strcat         -> fonction standard C, lstrcat en Windows
 *          Strcpyn        -> lstrcpyn en Windows
 *          Strcmpn        -> lstrcmpn en Windows
 *          Sprintf        -> fonction standard C, wsprintf en Windows 
 *          Calloc         -> Allocation de N*P octets
 *          Free           -> liberation des octets
 *          OutputDebugString -> fprintf (stderr, ...)
 *  et le fichier
 *          TCP4U_INCLUDE   -> include tcp4ux.h / tcp4w.h
 *
 * 
 * Note: Sous Windows certaines definitions sont inutiles pour tcp4w, mais ce
 * fichier est aussi utilise par d'autres projets...
 *
 */

/* ****************************************************************** */
/* Common Declarations                                                */
/* ****************************************************************** */


/* ****************************************************************** */
/* Declarations Windows                                               */
/* ****************************************************************** */

/* #include LIBCO_VERS_H */
#ifdef  _WINDOWS

#define  NEED_PROTO

#define WRITE_CR            0
#define Write(fd,buf,len)  _lwrite(fd,buf,len)
#define Open(fic,mode)     _lcreat(fic,mode)
#define Close(fd)          _lclose(fd)
#define Sprintf            wsprintf
#define Strlen             lstrlen
#define Strcpy             lstrcpy
#define Strcat             lstrcat
#define Strcpy             lstrcpy
#define Strcpyn            lstrcpyn
#define IsCancelled()      (WSAGetLastError()==WSAEINTR)
#define Calloc(n,s)         (void far *)GlobalAllocPtr(GMEM_SHARE | GMEM_ZEROINIT,n*s)
#define Free(p)             GlobalFreePtr (p)
#define IoctlSocket         ioctlsocket
#define CloseSocket(s)      closesocket(s)
#define Strstr(s1,s2)       strstr(s1,s2)                 
#define Vsprintf            wvsprintf

#define SYSTEM_EOL         "\r\n"

/* ----------------------------------- */
/* 16 bits declarations                */
/* ----------------------------------- */
#ifndef _WIN32
#  define Unlink(fic)        _unlink(fic)
#  define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#endif  /* Windows 3.1 */

/* ----------------------------------- */
/* 32 bits redeclarations              */
/* ----------------------------------- */
#ifdef _WIN32
#  define Unlink(fic)         DeleteFile(fic)
#  define _export
#  define GetCurrentTask()    GetCurrentThread()          
#  define WRITE               OF_WRITE
#  define READ                OF_READ
#  define IsTask(x)  (   GetThreadPriority(x)!= THREAD_PRIORITY_ERROR_RETURN \
              || GetLastError() != ERROR_INVALID_HANDLE)
#endif  /* Windows95 / Windows NT */


/* ----------------------------------- */
/* functions defined for compatibility */
/* ----------------------------------- */

HINSTANCE GetTaskInstance (HWND hParentWindow);
#endif /* _WINDOWS */


/* ****************************************************************** */
/* Declarations Unix                                                  */
/* ****************************************************************** */
#ifdef UNIX 

#define  GetCurrentTask()        (int) getpid ()
#define  WSACleanup()
#define  WSAGetLastError()       errno
#define  IsBadWritePtr(a,b)      FALSE
#define  WSAIsBlocking()         FALSE
#define  WSACancelBlockingCall()
#define  IsCancelled()          (errno==EINTR)
#define  Write(fd,buf,len)       write(fd,buf,len)
#define  Open(fic,mode)          open(fic,mode,0666) /* rw-rw-rw - umask */
#define  Close(fd)               close(fd)
#define  Sprintf                 sprintf
#define  Strlen                  strlen
#define  Strcpy                  strcpy
#define  Strcat                  strcat
#define  Strcpyn(a,b,n)          strncpy(a,b,n-1), a[n-1]=0
#ifdef __cplusplus  
#   define  Calloc(n,s)             (char *) calloc(n,s)
#else /* c++ */
#   define  Calloc(n,s)             calloc(n,s)
#endif /* not c++ */
#define  Free(p)                 free (p)
#define  IoctlSocket             ioctl
#define  CloseSocket(s)          close (s)
#define  Unlink(fic)             unlink(fic)
#define  Strstr(s1,s2)           strstr(s1,s2)                 
#define  Vsprintf                vsprintf
#define  OutputDebugString(x)    fputs (x, stderr)

#ifndef FALSE
#  define FALSE   (0==1)
#endif /* FALSE */
#ifndef TRUE
#  define TRUE    (1==1)
#endif /* TRUE */
#ifndef NULL
#  define NULL ((void *) 0)
#endif
#define CALLBACK 
#define FARPROC     HTTP4U_CALLBACK
#ifndef min
#  define min(a,b) ((a)<(b)?(a):(b))
#endif
#define WRITE_CR  (O_WRONLY | O_CREAT | O_TRUNC)
#define   UINT    unsigned int
#define   far

#define SYSTEM_EOL         "\r\n"

#ifndef TYPE_SOCKET_DEF
   typedef unsigned int SOCKET;
#define TYPE_SOCKET_DEF +
#endif /* TYPE_SOCKET_DEF */
#ifndef TYPE_HTASK_DEF
   typedef unsigned int HTASK;
#define TYPE_HTASK_DEF  +
#endif /* TYPE_HTASK_DEF */
#ifndef TYPE_BOOL_DEF
   typedef int BOOL;
#define TYPE_BOOL_DEF   +
#endif /* TYPE_BOOL_DEF */
#ifndef TYPE_HFILE_DEF
   typedef int HFILE;
#define TYPE_HFILE_DEF  +
#endif /* TYPE_HFILE_DEF */

#endif /* UNIX */

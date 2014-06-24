/*
 * Tcp4u v 3.31         Last Revision 27/02/1998  3.30.02
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    util.c
 * Purpose: A few utilities functions
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

/* this definition will be enough for our use */
#undef  toupper
#define toupper(x)  ( (x) & 0xDF )



/* ------------------------------------------------------------------------- */
/* Conversion d'un nom de machine en adresse IP                              */
/* Le nom est soit une "dotted address" soit un alias                        */ 
/* return : INADDR_NONE or IP address										 */ 
/* ------------------------------------------------------------------------- */
struct in_addr Tcp4uGetIPAddr (LPCSTR szHost)
{
struct hostent far *  lpHostEnt;
struct in_addr        sin_addr;

  Tcp4uLog (LOG4U_INTERN, "Tcp4uGetIPAddr host %s", szHost);

  sin_addr.s_addr = inet_addr (szHost); /* doted address */
  if (sin_addr.s_addr==INADDR_NONE)     /* si pas une doted address  */
  {                      /* regarder le fichier hosts */
      Tcp4uLog (LOG4U_CALL, "gethostbyname host %s", szHost);
      lpHostEnt = gethostbyname (szHost);
      if (lpHostEnt!=NULL)
         memcpy (& sin_addr.s_addr, lpHostEnt->h_addr, lpHostEnt->h_length);
      else       
      {
        Tcp4uLog (LOG4U_ERROR, "gethostbyname host %s", szHost);
      }

    }
return sin_addr;
} /* Tcp4uGetIPAddr */



/* -------------------------------------- */
/* a self-made atoi                       */
/* -------------------------------------- */

/* Note: ne pas utiliser la fonction isdigit: qui donne des */
/* resultats errones en Windows                             */
#define IsDigit(x)   ( (x)>='0' &&  (x)<='9')


int Tcp4uAtoi (LPCSTR p)
{
int n;
  Tcp4uLog (LOG4U_INTERN, "Tcp4uAtoi");

   for (n=0 ;  IsDigit (*p) ; p++)
     {
       n *= 10;
       n += (*p - '0');
     }
return n;   
} /* tcp4u_atoi */


long Tcp4uAtol (LPCSTR p)
{
long n;
  Tcp4uLog (LOG4U_INTERN, "Tcp4uAtol");

   for (n=0 ;  IsDigit (*p) ; p++)
     {
       n *= 10;
       n += (*p - '0');
     }
return n;   
} /* Tcp4uAtol */


/* a unix function which is not available under Windows */
/* should have same proto than strncmp                  */
/* adapted from the GNU strncasecmp                     */
int Tcp4uStrncasecmp (LPCSTR s1, LPCSTR s2, size_t n)
{
char c1, c2;
   for ( ; n!=0 ; --n)
   {
      c1 = *s1++;
      c2 = *s2++;
      if (toupper (c1)!=toupper(c2))    return  toupper (c1) - toupper(c2);
      if (c1==0)                        return 0;
   }
return  0;
} /* Strncasecmp */

/* ------------------------------------------------------------------------- */
/* a case-insentive strstr                                                   */
/* ------------------------------------------------------------------------- */
LPSTR Tcp4uStrIStr (LPCSTR s1, LPCSTR s2)
{
size_t nLength = Strlen(s2);
    for (;;)
    {
       while (*s1!=0  &&  toupper(*s1)!=toupper(*s2) )  s1++;
       if (*s1==0)   return NULL;
       if (Tcp4uStrncasecmp (s1, s2, nLength)==0)  return  (LPSTR) s1;
       s1++;
    }
} /* Tcp4uStrIStr  */

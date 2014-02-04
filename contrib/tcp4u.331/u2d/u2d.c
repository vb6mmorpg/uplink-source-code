/*
 * u2d v 1.10         Last Revision 02/03/1998  1.10
 *
 *===========================================================================
 *
 * Project: u2d,      Unix To Dos converter
 * File:    u2d.c
 * Purpose: u2d source code. 
 * Compilation vith microsoft C compiler:  
 * 		cl -O1 u2d.c setargv.obj 
 *
 *===========================================================================
 * 
 * This software is Copyright (c) 1996-1998 by Philippe Jounin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *  
 */   

#include <stdio.h>


int Convert (char *szFile)
{
#define XX_RETURN(s,x) \
{  printf ("%s\n", s) ; if (pFIn!=NULL) fclose(pFIn) ; \
   if (pFOut!=NULL)   fclose(pFOut);                   \
   if (TmpFile!=NULL) remove(TmpFile);                 \
   return x;   }
   
char *TmpFile=NULL;
int   c;
FILE *pFIn=NULL, *pFOut=NULL;

   printf ("%s", szFile);  fflush (stdout);
   TmpFile = tmpnam(NULL); 
   if (TmpFile==NULL) XX_RETURN ("\tCan not create temporary file", -1);
   pFOut = fopen (TmpFile, "w");
   if (pFOut==NULL) XX_RETURN ("\tCan not create temporary file", -2);
   pFIn = fopen (szFile, "r");
   if (pFIn==NULL) XX_RETURN ("\tCan not open input file", -3);
   
   while ( (c=fgetc (pFIn)) != EOF)
     { if (fputc (c, pFOut)==EOF)  XX_RETURN ("\tCan not write into output file", -4); }
   if (!feof (pFIn))  XX_RETURN ("\tCan not read from input file", -5);
   
   fclose (pFIn); fclose (pFOut);
   if (remove (szFile))  XX_RETURN ("\tCan not rename output file", -6);
   if (rename (TmpFile, szFile)==-1)
   { printf ("\tinput file %s removed, temp file %s can not be removed\n", 
   		      szFile, TmpFile);
     return -8; }
   printf ("\n");   
return 0;
   
} /* Convert */



int main (int argc, char *argv[])
{
  if (argc<2) 
    { 
       printf ("Usage: u2d file [file [...]]\n"
               "Translate a unix text file. Existing file is overwritten\n");
    }
  else
  {int Ark;
     for (Ark=1 ;  Ark<argc ; Ark++)
        Convert (argv[Ark]);
  }
return 0;       
} /* main */

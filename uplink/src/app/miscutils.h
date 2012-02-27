

/*
	Misc string/file utils

  */


#ifndef _included_miscutils_h
#define _included_miscutils_h

#include "tosser.h"
#include "options/options.h"

char *LowerCaseString       ( const char *thestring );
char *StripCarriageReturns  ( const char *thestring );               // Replaces first cr with \x0
char *TrimSpaces            ( const char *thestring );

char *GetFilePath           ( const char *filename );
void MakeDirectory          ( const char *directory );
bool DoesFileExist          ( const char *filename );
void EmptyDirectory         ( const char *directory );
bool CopyFilePlain          ( const char *oldfilename, const char *newfilename );
bool CopyFileUplink         ( const char *oldfilename, const char *newfilename );
bool RemoveFile             ( const char *filename );

DArray <char *> *ListDirectory  ( char *directory, char *filter );
DArray <char *> *ListSubdirs ( char *directory );

ColourOption *GetColour     ( char *colourName );
void SetColour              ( char *colourName );                    // calls glColour3f

void PrintStackTrace();

#endif


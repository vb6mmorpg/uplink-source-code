
/*

  B U N G L E 

	Zip file access library
	By Christopher Delay

  */


#ifndef _included_bungle_h
#define _included_bungle_h

#include <stdio.h>

#include "tosser.h"

//#ifdef _DEBUG
//#include "slasher.h"
//#endif


bool BglOpenZipFile ( char *zipfile, char *apppath, char *id = NULL );
bool BglOpenZipFile ( FILE *zipfile, char *apppath, char *id = NULL );

bool BglFileLoaded  ( char *filename );
bool BglExtractFile ( char *filename, char *target = NULL );

void BglCloseZipFile ( char *id );

void BglExtractAllFiles ( char *zipfile );

DArray <char *> *BglListFiles ( char *path, char *directory = NULL, char *filter = NULL );

void BglCloseAllFiles();

#endif


/*

  RedShirt library

	Designed to encrypt and decrypt sensitive files,
	such as save games or data files

  */


#ifndef _included_redshirt_h
#define _included_redshirt_h

#include <stdio.h>

#include "tosser.h"

//#ifdef _DEBUG
//#include "slasher.h"
//#endif


// ============================================================
// Basic routines =============================================


void RsInitialise ( char *apppath );                                            // Creates a temp directory
void RsCleanUp ();																// Deletes all created files



// ============================================================
// Encryption routines ========================================



bool RsFileExists		( char *filename );
bool RsFileEncrypted	( char *filename );
bool RsFileEncryptedNoVerify ( char *filename );


bool RsEncryptFile		( char *filename );						// Overwrites origional with encrypted
bool RsDecryptFile		( char *filename );						// Overwrites origional with decrypted


FILE *RsFileOpen		( char *filename, char *mode = "rb" );		// preserves origional
void  RsFileClose		( char *filename, FILE *file );



// ============================================================
// Archive file routines ======================================


bool RsLoadArchive			( char *filename );

FILE *RsArchiveFileOpen		( char *filename, char *mode );		      // Looks for file apppath/filename					
char *RsArchiveFileOpen		( char *filename );					      // Opens from filename first, then from zip file
bool RsArchiveFileLoaded	( char *filename );

void RsArchiveFileClose		( char *filename, FILE *file = NULL );

void RsCloseArchive			( char *filename );									// Frees all memory (how nice)

DArray <char *> *RsListArchive ( char *path = NULL, char *filter = NULL );

#endif

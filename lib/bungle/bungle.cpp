
#include <assert.h>

#include "tosser.h"

#include "bungle.h"

#include "mmgr.h"

//#include "debug.h"

struct LocalFileHeader
{

	char	signature [4];
	short	version;
	short	bitflag;
	short	compressionmethod;
	short	lastmodfiletime;
	short	lastmodfiledate;
	int		crc32;
	int		compressedsize;
	int		uncompressedsize;
	short	filenamelength;
	short	extrafieldlength;
	
	char	*filename;
	char	*extrafield;
	char	*data;

	char	*id;

};


static BTree <LocalFileHeader *> files;


void BglSlashify ( char *string )
{

	// Searches out all slashes and changes them to forward slashes

	char *next = strchr ( string, '\\' );

	while ( next ) {

		*(next) = '/';
		next = strchr ( next, '\\' );

	}

	// Also lowercases the string
	
	for ( char *p = string; *p != '\x0'; ++p )
		if ( *p >= 'A' && *p <= 'Z' )
			*p += 'a' - 'A';

}

bool BglOpenZipFile ( char *zipfile, char *apppath, char *id )
{

	FILE *file = fopen ( zipfile, "rb" );
	if ( !file ) return false;

	bool result = BglOpenZipFile ( file, apppath, id );

	fclose ( file );

	return result;

}

bool BglOpenZipFile ( FILE *file, char *apppath, char *id )
{
	
	if ( !file ) return false;

	while ( !feof ( file ) ) {

		LocalFileHeader *fh = new LocalFileHeader ();

		fread ( fh->signature, 4, 1, file );

		if ( fh->signature [0] != 'P' || fh->signature [1] != 'K' )	{				// No longer reading valid data
			delete fh;
			break;
		}

		fread ( &fh->version, 2, 1, file );
		fread ( &fh->bitflag, 2, 1, file );
		fread ( &fh->compressionmethod, 2, 1, file );
		fread ( &fh->lastmodfiletime, 2, 1, file );
		fread ( &fh->lastmodfiledate, 2, 1, file );
		fread ( &fh->crc32, 4, 1, file );
		fread ( &fh->compressedsize, 4, 1, file );
		fread ( &fh->uncompressedsize, 4, 1, file );
		fread ( &fh->filenamelength, 2, 1, file );
		fread ( &fh->extrafieldlength, 2, 1, file );

		if ( fh->filenamelength > 0 ) {
			fh->filename = new char [fh->filenamelength+1];
			fread ( fh->filename, fh->filenamelength, 1, file );
			*(fh->filename + fh->filenamelength) = 0;
		}
		else 
			fh->filename = NULL;

		if ( fh->extrafieldlength > 0 ) {
			fh->extrafield = new char [fh->extrafieldlength+1];
			fread ( fh->extrafield, fh->extrafieldlength, 1, file );
			*(fh->extrafield + fh->extrafieldlength) = 0;
		}
		else
			fh->extrafield = NULL;

		if ( fh->uncompressedsize > 0 ) {
			fh->data = new char [fh->uncompressedsize+1];
			fread ( fh->data, fh->uncompressedsize, 1, file );
			*(fh->data + fh->uncompressedsize) = 0;
		}
		else
			fh->data = NULL;

		if ( id ) {
			fh->id = new char [strlen(id) + 1];
			strcpy ( fh->id, id );
		}
		else 
			fh->id = NULL;

		if ( fh->compressionmethod == 0 &&
			 fh->compressedsize == fh->uncompressedsize &&
			 fh->filename ) {

			char fullfilename [256];
			sprintf ( fullfilename, "%s%s", apppath, fh->filename );

			BglSlashify ( fullfilename );

			files.PutData ( fullfilename, fh );

		}
		else {
			if ( fh->filename )
				delete [] fh->filename;
			if ( fh->extrafield )
				delete [] fh->extrafield;
			if ( fh->data )
				delete [] fh->data;
			if ( fh->id )
				delete [] fh->id;

			delete fh;
		}

	}

	return true;

}

bool BglFileLoaded ( char *filename )
{

	char *filenamecopy = new char [strlen(filename)+1];
	strcpy ( filenamecopy, filename );
	BglSlashify ( filenamecopy );

	LocalFileHeader *lfh = files.GetData ( filenamecopy );

	delete [] filenamecopy;

	return ( lfh != NULL );

}

void BglCloseZipFile_Recursive ( BTree <LocalFileHeader *> *files, 
								 LList <char *> *removableids, 
								 char *id )
{

	assert (removableids);
	assert (id);

	if ( !files ) return;										// Base case - end of the binary tree
	
	LocalFileHeader *lfh = files->data;

	if ( lfh && lfh->id && strcmp ( lfh->id, id ) == 0 ) {

		// This one is a match and should be flagged for removal
		removableids->PutData ( files->id );		

	}

	// Recurse

	BglCloseZipFile_Recursive ( files->Left (), removableids, id );
	BglCloseZipFile_Recursive ( files->Right (), removableids, id );

}

void BglCloseZipFile ( char *id )
{

	//
	// Create a DArray of filenames to be removed 
	// from our Binary Tree
	//

	LList <char *> removableids;

	//
	// Fill the DArray with a recursive search algorithm
	//

	BglCloseZipFile_Recursive ( &files, &removableids, id );

	//
	// Run through that DArray, looking up the file, 
	// deleting the data and removing it from the full btree
	//

	for ( int i = 0; i < removableids.Size (); ++i ) {
		
		char *filename = removableids.GetData (i);
		assert (filename);
		
		LocalFileHeader *lfi = files.GetData (filename);
		assert (lfi);

		files.RemoveData ( filename );

		if ( lfi->filename )	delete [] lfi->filename;
		if ( lfi->extrafield )	delete [] lfi->extrafield;
		if ( lfi->data )		delete [] lfi->data;
		if ( lfi->id )			delete [] lfi->id;

		delete lfi;
			
	}

}

bool BglExtractFile ( char *filename, char *target )
{

	char *filenamecopy = new char [strlen(filename)+1];
	strcpy ( filenamecopy, filename );
	BglSlashify ( filenamecopy );

	LocalFileHeader *lfh = files.GetData ( filenamecopy );

	delete [] filenamecopy;

	if ( lfh ) {

		FILE *output;
		
		if ( target )	output = fopen ( target, "wb" );
		else			output = fopen ( filename, "wb" );

		if ( !output ) return false;

		fwrite ( lfh->data, lfh->uncompressedsize, 1, output );

		fclose ( output );

		/*
		if ( target )
			DEBUG_PRINTF( "Written %s to %s\n", filename, target );
		else
			DEBUG_PRINTF( "Written %s to %s\n", filename, filename );
		*/

		return true;
	}		

	return false;

}


void BglExtractAllFiles ( char *zipfile )
{

	FILE *file = fopen ( zipfile, "rb" );
	assert (file);

	while ( !feof ( file ) ) {

		LocalFileHeader fh;

		fread ( &fh.signature, 4, 1, file );
		fread ( &fh.version, 2, 1, file );
		fread ( &fh.bitflag, 2, 1, file );
		fread ( &fh.compressionmethod, 2, 1, file );
		fread ( &fh.lastmodfiletime, 2, 1, file );
		fread ( &fh.lastmodfiledate, 2, 1, file );
		fread ( &fh.crc32, 4, 1, file );
		fread ( &fh.compressedsize, 4, 1, file );
		fread ( &fh.uncompressedsize, 4, 1, file );
		fread ( &fh.filenamelength, 2, 1, file );
		fread ( &fh.extrafieldlength, 2, 1, file );

		if ( fh.filenamelength > 0 ) {
			fh.filename = new char [fh.filenamelength];
			fread ( fh.filename, fh.filenamelength, 1, file );
			*(fh.filename + fh.filenamelength) = 0;
		}
		else 
			fh.filename = NULL;

		if ( fh.extrafieldlength > 0 ) {
			fh.extrafield = new char [fh.extrafieldlength+1];
			fread ( fh.extrafield, fh.extrafieldlength, 1, file );
			*(fh.extrafield + fh.extrafieldlength) = 0;
		}
		else
			fh.extrafield = NULL;

		if ( fh.uncompressedsize > 0 ) {
			fh.data = new char [fh.uncompressedsize];
			fread ( fh.data, fh.uncompressedsize, 1, file );
			*(fh.data + fh.uncompressedsize) = 0;
		}
		else
			fh.data = NULL;


		if ( fh.compressionmethod == 0 &&
			 fh.compressedsize == fh.uncompressedsize &&
			 fh.filename ) {

			FILE *output = fopen ( fh.filename, "wb" );
			assert (output);

			fwrite ( fh.data, fh.uncompressedsize, 1, output );

			fclose ( output );

		}

	}

	fclose ( file );

}


DArray <char *> *BglListFiles ( char *path, char *directory, char *filter )
{

    char dirCopy [256];
    sprintf ( dirCopy, "%s%s", path, directory );
    BglSlashify ( dirCopy );

    DArray <char *> *result = files.ConvertIndexToDArray();

    for ( int i = 0; i < result->Size(); ++i ) {
        if ( result->ValidIndex(i) ) {
        
            char *fullPath = result->GetData(i);
            char thisDir[256];
            strncpy( thisDir, fullPath, strlen(dirCopy) );
            thisDir[strlen(dirCopy)] = '\x0';

            bool removeMe = false;

            if ( strcmp ( thisDir, dirCopy ) != 0 )         removeMe = true;                
            if ( strstr ( fullPath, filter ) == NULL )      removeMe = true;

            if ( removeMe )
                result->RemoveData( i );

        }
    }

    return result;

}


void BglCloseAllFiles( BTree<LocalFileHeader *> *files )
{
	if ( !files ) return;

	BglCloseAllFiles( files->Left() );
	BglCloseAllFiles( files->Right() );

	LocalFileHeader *lfi = files->data;
	if ( lfi ) {
		if ( lfi->filename )	delete [] lfi->filename;
		if ( lfi->extrafield )	delete [] lfi->extrafield;
		if ( lfi->data )		delete [] lfi->data;
		if ( lfi->id )			delete [] lfi->id;
		delete lfi;
	}

	files->Empty();
}

void BglCloseAllFiles()
{
	BglCloseAllFiles( &files );
}

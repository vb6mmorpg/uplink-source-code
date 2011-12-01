// -*- tab-width:4 c-file-style:"cc-mode" -*-

#include "stdafx.h"

#ifdef WIN32
#include <direct.h>
#include <io.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h> /* glu extention library */

#include "redshirt.h"

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"

#include "options/options.h"

#include "mmgr.h"


char *GetFilePath ( const char *filename )
{

	UplinkAssert (filename);

	size_t newstringsize = strlen (filename) + 1;
    char *newstring = new char [newstringsize];
    UplinkStrncpy ( newstring, filename, newstringsize );
/*    
    char *p = newstring;
    
    while ( strchr ( p, '/' ) )	
		p = strchr ( p, '/' ) + 1;
	    
    if (p) *p = '\x0';
  */
    
    char     *p = strrchr ( newstring, '/' );
    if ( !p ) p = strrchr ( newstring, '\\' );

    if ( p ) *(p+1) = '\x0';
    else     UplinkStrncpy ( newstring, "./", newstringsize );

    return newstring;    
    
}

char *LowerCaseString ( const char *thestring )
{

	UplinkAssert (thestring);
	
	char *thecopy = new char [strlen(thestring)+1];
	UplinkSafeStrcpy ( thecopy, thestring );

	for ( char *p = thecopy; *p != '\x0'; ++p )
		if ( *p >= 'A' && *p <= 'Z' )
			*p += 'a' - 'A';

	return thecopy;

}

char *StripCarriageReturns ( const char *thestring )
{

    UplinkAssert (thestring);

    // Is there a cr?

    const char *firstcr = strchr ( thestring, '\n' );

    if ( !firstcr ) {

        // No cr found - copy string and return it
     
        char *result = new char [strlen(thestring)+1];
        UplinkSafeStrcpy ( result, thestring );
        return result;

    }
    else {

        // Found a cr - so shorten string to that point

        size_t newlength = firstcr - thestring;
        char *result = new char [newlength+1];
        strncpy ( result, thestring, newlength );
        result [newlength] = '\x0';
        return result;

    }

}

char *TrimSpaces ( const char *thestring )
{

    UplinkAssert (thestring);

	char *retstring;

	int indexfirstnospace = -1;
	int indexlastnospace = -1;

	for ( int i = 0; thestring [ i ] != '\0'; i++ ) {
		if ( indexlastnospace == -1 )
			indexfirstnospace = i;
		if ( thestring [ i ] != ' ' )
			indexlastnospace = i;
	}

	int lenretstring;
	if ( indexfirstnospace == -1 || indexlastnospace == -1 )
		lenretstring = 0;
	else
		lenretstring = ( indexlastnospace - indexfirstnospace ) + 1;

	retstring = new char [ lenretstring + 1 ];
	if ( lenretstring > 0 )
		memcpy ( retstring, thestring + indexfirstnospace, lenretstring );
	retstring [ lenretstring ] = '\0';

	return retstring;

}

void MakeDirectory ( const char *dirname )
{

#ifdef WIN32
  _mkdir ( dirname );
#else
  mkdir ( dirname, 0700 );
#endif

}


bool DoesFileExist ( const char *filename )
{

    return access(filename, 0) == 0;

}

void EmptyDirectory ( const char *directory )
{

#ifdef WIN32

	char searchstring [_MAX_PATH + 1];
	UplinkSnprintf ( searchstring, sizeof ( searchstring ), "%s*", directory ); 

	_finddata_t thisfile;
	intptr_t fileindex = _findfirst ( searchstring, &thisfile );

	int exitmeplease = 0;

	while ( fileindex != -1 && !exitmeplease ) {

		if ( strcmp ( thisfile.name, "." ) != 0 && strcmp ( thisfile.name, ".." ) != 0 ) {
			char newname [_MAX_PATH + 1];
			UplinkSnprintf ( newname, sizeof ( newname ), "%s%s", directory, thisfile.name );      
			_unlink ( newname );
		}
		exitmeplease = _findnext ( fileindex, &thisfile );

	}

	if ( fileindex != -1 )
		_findclose ( fileindex );

#else

	char userdir [256];
	UplinkStrncpy ( userdir, directory, sizeof ( userdir ) );
	DIR *dir = opendir( userdir );
	if (dir != NULL) {
	    struct dirent *entry = readdir ( dir );

	    while (entry != NULL) {
	    
			if ( strcmp ( entry->d_name, "." ) != 0 && strcmp ( entry->d_name, ".." ) != 0 ) {
				char newname [256];
				UplinkSnprintf ( newname, sizeof ( newname ), "%s%s", directory, entry->d_name );      
				unlink ( newname );
			}
		    entry = readdir ( dir );
	    
	    }
	  
	    closedir( dir );
	}
#endif

}

bool CopyFilePlain ( const char *oldfilename, const char *newfilename )
{

	bool success = false;
	FILE *fileread = fopen ( oldfilename, "rb" );
	FILE *filewrite = fopen ( newfilename, "wb" );

	if ( fileread && filewrite ) {
		char buffer [256];
		size_t sizeread;

		while ( ( sizeread = fread ( buffer, 1, sizeof ( buffer ), fileread ) ) > 0 ) {
			fwrite ( buffer, 1, sizeread, filewrite );
		}

		success = true;
	}

	if ( filewrite )
		fclose ( filewrite );
	if ( fileread )
		fclose ( fileread );

	return success;

}

bool CopyFileUplink ( const char *oldfilename, const char *newfilename )
{

	bool success = false;
	FILE *fileread = RsFileOpen ( const_cast<char *>( oldfilename ) );
	FILE *filewrite = fopen ( newfilename, "wb" );

	if ( fileread && filewrite ) {
		char buffer [256];
		size_t sizeread;

		while ( ( sizeread = fread ( buffer, 1, sizeof ( buffer ), fileread ) ) > 0 ) {
			fwrite ( buffer, 1, sizeread, filewrite );
		}

		success = true;
	}

	if ( filewrite )
		fclose ( filewrite );
	if ( fileread )
		RsFileClose ( const_cast<char *>( oldfilename ), fileread );

	return success;

}

bool RemoveFile ( const char *filename )
{

#ifdef WIN32
	return ( _unlink ( filename ) == 0 );
#else
	return ( unlink ( filename ) == 0 );
#endif

}

DArray <char *> *ListDirectory  ( char *directory, char *filter )
{

    //
    // Start with a listing from Redshirt archives

    DArray <char *> *result = RsListArchive ( directory, filter );

    //
    // Now add on all files found locally

#ifdef WIN32

	char searchstring [_MAX_PATH + 1];
	UplinkSnprintf ( searchstring, sizeof ( searchstring ), "%s%s*%s", app->path, directory, filter ); 

	_finddata_t thisfile;
	intptr_t fileindex = _findfirst ( searchstring, &thisfile );

	int exitmeplease = 0;

	while ( fileindex != -1 && !exitmeplease ) {

		size_t newnamesize = _MAX_PATH + 1;
		char *newname = new char [newnamesize];
		UplinkSnprintf ( newname, newnamesize, "%s%s", directory, thisfile.name );      
        result->PutData ( newname );
		exitmeplease = _findnext ( fileindex, &thisfile );

	}

	if ( fileindex != -1 )
		_findclose ( fileindex );

#else

	char userdir [256];
	//UplinkStrncpy ( userdir, directory, sizeof ( userdir ) );
	UplinkSnprintf ( userdir, sizeof ( userdir ), "%s%s", app->path, directory ); 
	DIR *dir = opendir( userdir );
	if (dir != NULL) {
	    struct dirent *entry = readdir ( dir );

	    while (entry != NULL) {
	    
		    char *p = strstr(entry->d_name, filter);
		    if ( p ) {
				size_t newnamesize = 256;
		        char *newname = new char [newnamesize];
		        UplinkSnprintf ( newname, newnamesize, "%s%s", directory, entry->d_name );
		        result->PutData ( newname );
		    }
	        
		    entry = readdir ( dir );
	    
	    }
	  
	    closedir( dir );
	}
#endif

    //
    // Now make absolutely sure there are no duplicates

    for ( int i = 0; i < result->Size(); ++i ) {
        if ( result->ValidIndex(i) ) {

            for ( int j = i + 1; j < result->Size(); ++j ) {
                if ( result->ValidIndex(j) ) {

                    char *resultI = result->GetData(i);
                    char *resultJ = result->GetData(j);

					if ( strcmp ( resultI, resultJ ) == 0 ) {
						delete [] resultJ;
                        result->RemoveData( j );
					}

                }
            }

        }
    }

    //
    // All done

    return result;

}

DArray <char *> *ListSubdirs ( char *directory )
{

    DArray <char *> *result = new DArray <char *> ();

    //
    // Now add on all files found locally

#ifdef WIN32

	_finddata_t thisfile;
	char filter[256];
	UplinkSnprintf ( filter, sizeof ( filter ), "%s*.*", directory );
	intptr_t fileindex = _findfirst ( filter, &thisfile );

	int exitmeplease = 0;

	while ( fileindex != -1 && !exitmeplease ) {
		if ( thisfile.attrib & _A_SUBDIR &&
			 strcmp ( thisfile.name, "." ) != 0 &&
			 strcmp ( thisfile.name, ".." ) != 0 ) {		

			size_t newnamesize = 256;
			char *newname = new char [newnamesize];
			UplinkStrncpy ( newname, thisfile.name, newnamesize );
			result->PutData ( newname );

		}
		exitmeplease = _findnext ( fileindex, &thisfile );
	}

	if ( fileindex != -1 )
		_findclose ( fileindex );

#else

	DIR *dir = opendir(directory);
	if (dir != NULL) {
		for (struct dirent *d; (d = readdir(dir)) != NULL;) {
			char fullfilename[256];
			struct stat info;

			UplinkSnprintf(fullfilename, sizeof ( fullfilename ), "%s/%s", directory, d->d_name);
			
			if (stat(fullfilename, &info) == 0 
				&& S_ISDIR(info.st_mode) 
				&& d->d_name[0] != '.') { 
					char *newname = new char [strlen(d->d_name) + 1];
					UplinkSafeStrcpy(newname, d->d_name);
					result->PutData( newname );
			}
		}
		closedir(dir);
	}
#endif

	return result;

}

void SetColour ( char *colourName )
{

    if ( !app || 
         !app->GetOptions () || 
         !app->GetOptions()->GetColour( colourName ) ) {

        printf ( "SetColour WARNING : Failed to find colour %s\n", colourName );
        glColor3f ( 0.0f, 0.0f, 0.0f );
        return;

    }

    ColourOption *col = app->GetOptions ()->GetColour ( colourName );
    UplinkAssert (col);
    glColor3f ( col->r, col->g, col->b );

}


unsigned *getRetAddress(unsigned *mBP)
{
#ifdef WIN32
	unsigned *retAddr;

	__asm {
		mov eax, [mBP]
		mov eax, ss:[eax+4];
		mov [retAddr], eax
	}

	return retAddr;
#else
	unsigned **p = (unsigned **) mBP;
	return p[1];
#endif
}

void PrintStackTrace()
{
// TODO: Make this work in a sane way on all platforms
}


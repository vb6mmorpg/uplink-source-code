
#include <stdio.h>

#include "sgplaylist.h"

#include "mmgr.h"



SgPlaylist::SgPlaylist ()
{

    sprintf ( name, "New playlist" );
            
}

SgPlaylist::~SgPlaylist ()
{

	for ( int i = 0; i < songs.Size (); ++i ) 
		if ( songs.GetData (i) )
			if ( strlen(songs.GetData (i)) != 0 )
				if ( strcmp(songs.GetData (i), "") != 0 )
					delete [] songs.GetData (i);

}


void SgPlaylist::SetName ( char *newname )
{

    strcpy ( name, newname );

}

void SgPlaylist::AddSong ( char *name )
{

    char *namecopy = new char [strlen(name)+1];
    sprintf ( namecopy, name );
    songs.PutData(namecopy);

}

int SgPlaylist::NumSongs ()
{
    
    return songs.Size ();

}

char *SgPlaylist::GetRandomSong ()
{

  	int songindex = (int)( ( (float) rand () / (float) RAND_MAX ) * NumSongs () );
    return songs.GetData(songindex);

}

char *SgPlaylist::GetRandomSong ( char *oldsong )
{

    if ( !oldsong ) {

        return GetRandomSong ();

    }
    else if ( NumSongs () == 1 ) {

        return songs.GetData (0);

    }
    else {

        char *candidate = GetRandomSong ();
        while ( strcmp ( candidate, oldsong ) == 0 )
            candidate = GetRandomSong ();

        return candidate;

    }

}

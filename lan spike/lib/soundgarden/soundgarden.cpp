
/*

  Platform independant Soundgarden functions

  */


#include "soundgarden.h"
#include "sgplaylist.h"
#include "tosser.h"
#include "redshirt.h"
#include "eclipse.h"

#include "mmgr.h"


static LList <SgPlaylist *> playlists;
static char currentplaylist [SIZE_SGPLAYLIST_NAME];
static char currentsong [256];

static char requestedplaylist [SIZE_SGPLAYLIST_NAME];
static int requestedtime;

static int songindex;


void SgPlaylist_Initialise ()
{

    sprintf ( currentplaylist, "None" );
    sprintf ( currentsong, "None" );
    sprintf ( requestedplaylist, "None" );
    requestedtime = -1;
    songindex = -1;

}

void SgPlaylist_Shutdown ()
{

    while ( playlists.GetData (0) ) {

        SgPlaylist *playlist = playlists.GetData (0);
        playlists.RemoveData (0);
        delete playlist;

    }

    playlists.Empty ();

}

void SgPlaylist_Create ( char *pname )
{

    SgPlaylist *playlist = new SgPlaylist ();
    playlist->SetName ( pname );
    playlists.PutData ( playlist );

}

SgPlaylist *SgPlaylist_GetPlaylist ( char *pname )
{

    for ( int i = 0; i < playlists.Size (); ++i ) 
        if ( playlists.GetData (i) )            
            if ( strcmp ( playlists.GetData (i)->name, pname ) == 0 )
                return playlists.GetData(i);

    return NULL;

}

void SgPlaylist_AddSong ( char *pname, char *songname )
{

    SgPlaylist *playlist = SgPlaylist_GetPlaylist(pname);

    if ( playlist ) 
        playlist->AddSong (songname);

    else
        printf ( "SgPlaylist_AddSong : Failed because playlist %s does not exist\n", pname );

}

void SgPlaylist_Play ( char *pname )
{

    if ( strcmp ( currentplaylist, "None" ) == 0 ) {

        // Nothing is currently playing
        // Start the new playlist immediately

        strcpy ( currentplaylist, pname );
//        SgSetModVolume ( 100 );
        songindex = -1;
        SgPlaylist_NextSong ();

    }
    else if ( strcmp ( currentplaylist, pname ) != 0 ) {

        // Something is currently playing
        // Fade it out slowly before changing

        if ( strcmp ( requestedplaylist, "None" ) == 0 )
            requestedtime = (int)EclGetAccurateTime ();

        strcpy ( requestedplaylist, pname );
            
    }

}

void SgPlaylist_Stop ()
{

    SgStopMod ();
    sprintf ( currentplaylist, "None" );
    sprintf ( currentsong, "None" );
    sprintf ( requestedplaylist, "None" );
    requestedtime = -1;
    songindex = -1;

}

void SgPlaylist_NextSong ()
{

    SgPlaylist *playlist = SgPlaylist_GetPlaylist(currentplaylist);

    if ( playlist ) {

        ++songindex;
        if ( songindex >= playlist->songs.Size() ) songindex = 0;
        char *songtitle = playlist->songs.GetData(songindex);

        SgPlayMod ( RsArchiveFileOpen ( songtitle ) );
        strcpy ( currentsong, songtitle );

    }
    else
        printf ( "SgPlaylist_NextSong : Failed because playlist %s does not exist\n", currentplaylist );


}

void SgPlaylist_RandomSong ()
{

    SgPlaylist *playlist = SgPlaylist_GetPlaylist(currentplaylist);

    if ( playlist ) {

        char *songtitle = playlist->GetRandomSong ( currentsong );
        SgPlayMod ( RsArchiveFileOpen ( songtitle ) );
        strcpy ( currentsong, songtitle );

    }
    else
        printf ( "SgPlaylist_RandomSong : Failed because playlist %s does not exist\n", currentplaylist );

}

void SgUpdate ()
{

    // Are we fading out the current playlist?

    if ( strcmp ( requestedplaylist, "None" ) != 0 ) {

        int timediff = (int)EclGetAccurateTime () - requestedtime;
        int volume = 20 - (int)(20 * ((float) timediff / (float) 4000));

        if ( volume <= 0 ) {
            SgStopMod ();
            strcpy ( currentplaylist, requestedplaylist );
            strcpy ( requestedplaylist, "None" );
            requestedtime = -1;
            songindex = -1;
            SgSetModVolume ( 20 );
            SgPlaylist_NextSong ();
        }
        else {
            SgSetModVolume ( volume );
        }

    }
    else {

        // Playing a playlist as usual
        // Has the current mod finished?

        if ( strcmp ( currentplaylist, "None" ) != 0 )
            if ( SgModFinished () ) 
                SgPlaylist_NextSong ();

    }

}

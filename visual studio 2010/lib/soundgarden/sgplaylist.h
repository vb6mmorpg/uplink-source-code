
/*

  SgPlaylist class object

    Stores a list of song names 
    to be played for a particular mood

  */


#ifndef _included_sgplaylist_h
#define _included_sgplaylist_h

#define SIZE_SGPLAYLIST_NAME 64

#include "tosser.h"

//#ifdef _DEBUG
//#include "slasher.h"
//#endif


class SgPlaylist
{

public:

    char name [SIZE_SGPLAYLIST_NAME];
    LList <char *> songs;

public:

    SgPlaylist ();
    ~SgPlaylist ();

    void SetName ( char *newname );
    void AddSong ( char *name );

    int NumSongs ();
    char *GetRandomSong ();
    char *GetRandomSong ( char *oldsong );                      // Any song but not the old one
                                                                // Warning - infinite loop if all songs have same name
};



#endif


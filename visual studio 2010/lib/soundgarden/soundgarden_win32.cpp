/*
    Sound garden sound library
    By Christopher Delay

*/

// Now use soundgarden_sdlmixer instead
//#ifdef WIN32
#if 0

#include <windows.h>
#include <winbase.h>

#include <stdio.h>
#include <stdarg.h>
#include <direct.h>

#include "tosser.h"
#include "soundgarden.h"

extern "C" 
{
#include "mikmod.h"
}

#include "nommgr.h"

static UNIMOD *currentmod = NULL;
static int currentwavchannel = 16;
static BTree <SAMPLE *> cache;					// Stores sounds already loaded


static void SgDebugPrintf( const char *fmt, ... )
{
#ifdef _DEBUG
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt,ap);
    va_end(ap);
#endif
}

void SGtickhandler(void)
{
    // Used by MikMod to keep music playing 

    if ( currentmod ) {

        MP_HandleTick();    /* play 1 tick of the module */
 
    }

}

void SgInitialise ()
{

    // MIKMOD setup

    /*
      Initialize soundcard parameters.. you _have_ to do this
      before calling MD_Init(), and it's illegal to change them
      after you've called MD_Init()
    */

    md_mixfreq      =44100;                     /* standard mixing freq */
    md_dmabufsize   =20000;                     /* standard dma buf size */
    md_mode         =DMODE_16BITS|DMODE_STEREO|DMODE_INTERP ; /* standard mixing mode */
    md_device       =0;                                       /* standard device: autodetect */

    mp_loop         =0;                     // Dont loop

    /*
      Register the loaders we want to use..
    */

//	ML_RegisterLoader(&load_m15);    /* if you use m15load, register it as first! */
  ML_RegisterLoader(&load_mod);
//	ML_RegisterLoader(&load_mtm);
  ML_RegisterLoader(&load_s3m);
//	ML_RegisterLoader(&load_stm);
//	ML_RegisterLoader(&load_ult);
    ML_RegisterLoader(&load_uni);
  ML_RegisterLoader(&load_xm);

    /*
      Register the drivers we want to use:
    */

    MD_RegisterDriver(&drv_nos);
    MD_RegisterDriver(&drv_ds);

    MD_RegisterPlayer(SGtickhandler);

    /*  initialize soundcard */

    if(!MD_Init()){
	    SgDebugPrintf("MikMod Driver error: %s.\n",myerr);	
        return;
    }

    /*
      set the number of voices to use.. you
      could add extra channels here (e.g. md_numchn=mf->numchn+4; )
      to use for your own soundeffects:
    */

    md_numchn = 24;
   
}

void SgShutdown ()
{

	SgStopMod ();
	MD_Exit ();

    DArray <SAMPLE *> *d_cache = cache.ConvertToDArray ();
    
	for ( int i = 0; i < d_cache->Size (); ++i ) {
		if ( d_cache->ValidIndex (i) ) {
			SAMPLE * sample = d_cache->GetData( i );
			if ( sample ) {
				free( sample ); // Not ideal, but MW_FreeWav( sample ) crashes.
			}
		}
	}

    delete d_cache;
    cache.Empty ();

    SgPlaylist_Shutdown ();

}

void SgPlaySound ( char *filename, char *id, bool synchronised )
{

    if ( !filename ) {
        SgDebugPrintf ( "SoundGarden WARNING : Tried to play sound (NULL)\n" );
        return;
    }

	char *fullfilename = filename;

    SAMPLE *sample = NULL;
    char *sampleid = id ? id : filename;

    if ( cache.LookupTree ( sampleid ) ) {

        // Sound sample already loaded into memory - start it playing
		
        sample = cache.GetData ( sampleid );
		
        if ( !sample ) {

	        SgDebugPrintf ( "SoundGarden WARNING : Failed to load sound from cache : %s\n", filename );
            cache.RemoveData ( sampleid );
            return;

        }

    }
    else {

        // Load sample and place into cache

        sample = MW_LoadWavFN( fullfilename );
        if ( !sample ) {

            SgDebugPrintf ( "SoundGarden WARNING : failed to load sound %s\n (%s)", fullfilename, myerr );
            return;

        }

        cache.PutData ( sampleid, sample );

    }

    MD_PlayStart ();

    MD_VoiceSetVolume(currentwavchannel,64);
    MD_VoiceSetPanning(currentwavchannel,128);
    MD_VoiceSetFrequency(currentwavchannel,22000);
    MD_VoicePlay(currentwavchannel,sample->handle,0,sample->length,0,0,sample->flags);	
    ++currentwavchannel;
    if (currentwavchannel > 23) currentwavchannel = 16;

}

void SgPlayMod ( char *filename )
{

    SgStopMod ();

    char *fullfilename = filename;

    currentmod=ML_LoadFN(fullfilename);

    /* didn't work -> exit with errormsg. */

    if(currentmod==NULL){
        
		SgDebugPrintf("SoundGarden WARNING : Failed to load music file %s\n (%s)\n", fullfilename, myerr);
        return;

    }

    /*      initialize modplayer to play this module */

    MP_Init(currentmod);
    //MD_SetBPM(mp_bpm);

    /*  start playing the module: */

    MD_PlayStart();
    SgDebugPrintf ( "Playing Music : %s (%d channels)\n", filename, currentmod->numchn );

}

void SgSetModVolume ( int newvolume )
{

	mp_volume = newvolume;

}

int SgGetModVolume ()
{

    return mp_volume;

}

bool SgModFinished ()
{

    return !currentmod || MP_Ready ();

}

void SgStopMod ()
{
    if (currentmod) {
		MD_PlayStop();          /* stop playing */
		ML_Free(currentmod);            /* and free the module */
		currentmod = NULL;
    }
}

#endif

/*
	Sound garden sound library
	By Christopher Delay

  */

#ifndef WIN32
#ifndef USE_SDL

#include "tosser.h"
#include "soundgarden.h"

#include <mikmod.h>

#if LIBMIKMOD_VERSION < 0x030109 
#error "SoundGarden requires libmikmod >= 3.1.9"
#endif

#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include <unistd.h>

static MODULE *currentmod = NULL;
static BTree <SAMPLE *> cache;					// Stores sounds already loaded

static pthread_t mikmod_update_thread;
static bool sg_thread_termination_requested = false;

static void SgDebugPrintf( const char *fmt, ... );
static void * SgUpdateMikModThread( void *arg );

static int playerVolume = 128;

void SgInitialise ()
{
    // MikMod Setup
    
    MikMod_RegisterAllLoaders();
    
    MikMod_RegisterDriver(&drv_alsa);
    MikMod_RegisterDriver(&drv_oss);
    MikMod_RegisterDriver(&drv_esd);
    MikMod_RegisterDriver(&drv_nos);

    if(MikMod_Init("") != 0){
        SgDebugPrintf("MikMod Driver error: %s.\n",MikMod_strerror(MikMod_errno));	
        return;
    }
    
    if(MikMod_InitThreads() != 1){
        SgDebugPrintf("MikMod Not Threadsafe: %s.\n",MikMod_strerror(MikMod_errno));	
        return;
    }
    
    if (MikMod_SetNumVoices(16,8) != 0) 
        SgDebugPrintf("SoundGarden WARNING: Failed to set the desired number of voices\n");
    
    MikMod_EnableOutput();
            
    // Start the Sound Thread

    if (pthread_create( &mikmod_update_thread, NULL, SgUpdateMikModThread, NULL ) != 0) 
        SgDebugPrintf("SoundGarden WARNING: Failed to create sound thread\n");
}

void SgShutdown()
{
    SgStopMod();
    MikMod_DisableOutput();
    
    sg_thread_termination_requested = true;
    pthread_join(mikmod_update_thread, NULL);
    
    MikMod_Exit();
}

void SgPlaySound ( char *fullfilename, char *id, bool synchronised )
{
    SAMPLE *sample = NULL;
    char *sampleid = id ? id : fullfilename;

    if ( cache.LookupTree ( sampleid ) ) {

        // Sound sample already loaded into memory - start it playing
		
        sample = cache.GetData ( sampleid );
		
        if ( !sample ) {

            SgDebugPrintf ( "SoundGarden WARNING : Failed to load sound from cache : %s\n", fullfilename );
            cache.RemoveData ( sampleid );
            return;

        }

    }
    else {

        // Load sample and place into cache

        sample = Sample_Load( fullfilename );
        if ( !sample ) {
            SgDebugPrintf ( "SoundGarden WARNING : failed to load sound %s\n (%s)", fullfilename, MikMod_strerror(MikMod_errno) );
            return;
        }

        cache.PutData ( sampleid, sample );

    }
    
    int voice = Sample_Play(sample, 0, SFX_CRITICAL);
    
    Voice_SetVolume(voice,128);
    Voice_SetPanning(voice,PAN_CENTER);
    Voice_SetFrequency(voice,22000);

}

void SgPlayMod ( char *fullfilename )
{
    SgStopMod ();
    
    if ( !currentmod ) {

        currentmod=Player_Load(fullfilename, 16, 0);

        /* didn't work -> exit with errormsg. */

        if(currentmod==NULL){
            SgDebugPrintf("SoundGarden WARNING : Failed to load music file %s\n (%s)\n", fullfilename, MikMod_strerror(MikMod_errno));
            return;
        }

        /*  start playing the module: */
        
        Player_Start(currentmod);
        Player_SetVolume(playerVolume);
        
        SgDebugPrintf ( "SoundGarden Playing Music : %s (%d channels)\n", fullfilename, currentmod->numchn );

    }
}

void SgSetModVolume ( int newvolume )
{
    SgDebugPrintf ( "SoundGarden Music Volume: %d\n", newvolume );
    
    playerVolume = newvolume;
    Player_SetVolume( newvolume );
}

void SgStopMod ()
{
    if (currentmod) {
        Player_Stop();              /* stop playing */
        Player_Free(currentmod);   /* and free the module */
        currentmod = NULL;
    }
}

bool SgModFinished ()
{
    return !currentmod || !Player_Active ();
}

static void SgDebugPrintf( const char *fmt, ... )
{
#ifdef _DEBUG
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt,ap);
    va_end(ap);
#endif
}

static void * SgUpdateMikModThread( void *arg )
{
    while (!sg_thread_termination_requested) {
        usleep(10000);
        
        MikMod_Update();
    }
    
    return 0;
}

#endif
#endif

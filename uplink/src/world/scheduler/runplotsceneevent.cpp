
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"

#include "world/world.h"
#include "world/scheduler/runplotsceneevent.h"

#include "mmgr.h"



RunPlotSceneEvent::RunPlotSceneEvent ()
{
	
	act = 0;
	scene = 0;

}

RunPlotSceneEvent::~RunPlotSceneEvent ()
{
}

void RunPlotSceneEvent::SetActAndScene ( int newact, int newscene )
{

	act = newact;
	scene = newscene;

}

void RunPlotSceneEvent::Run ()
{

    if ( act != -1 )
	    game->GetWorld ()->plotgenerator.Run_Scene ( act, scene );

    else
        game->GetWorld ()->demoplotgenerator.RunScene ( scene );

}


char *RunPlotSceneEvent::GetShortString ()
{
	
	return GetLongString ();

}

char *RunPlotSceneEvent::GetLongString ()
{

	size_t resultsize = 32;
	char *result = new char [resultsize];

	if ( act != -1 ) {
	    UplinkSnprintf ( result, resultsize, "Run Plot Act %d, Scene %d", act, scene );

	} else {
        UplinkSnprintf ( result, resultsize, "Run Demo Plot Scene %d", scene );
	}

	return result;

}


bool RunPlotSceneEvent::Load  ( FILE *file )
{

	LoadID ( file );

    if ( !UplinkEvent::Load ( file ) ) return false;

	if ( !FileReadData ( &act, sizeof(act), 1, file ) ) return false;
	if ( !FileReadData ( &scene, sizeof(scene), 1, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void RunPlotSceneEvent::Save  ( FILE *file )
{

	SaveID ( file );

    UplinkEvent::Save ( file );

	fwrite ( &act, sizeof(act), 1, file );
	fwrite ( &scene, sizeof(scene), 1, file );

	SaveID_END ( file );
}

void RunPlotSceneEvent::Print ()
{

    UplinkEvent::Print ();
	printf ( "%s\n", GetLongString () );

}

	
char *RunPlotSceneEvent::GetID ()
{
	
	return "EVT_PLOT";

}

int  RunPlotSceneEvent::GetOBJECTID ()
{
	
	return OID_RUNPLOTSCENEEVENT;

}



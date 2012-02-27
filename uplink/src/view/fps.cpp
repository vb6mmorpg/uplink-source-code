
/*

  Frame rate checker utility class source file
  By Christopher Delay

  */

#include <stdio.h>

#include "eclipse.h"

#include "view/fps.h"

#include "mmgr.h"


FrameRate :: FrameRate ()
{

    numframesremaining = NUMFRAMES;
    fps = 0.0;

}

FrameRate :: ~FrameRate ()
{
}

void FrameRate :: Start ()
{

	starttime = (int) EclGetAccurateTime ();
	numframesremaining = NUMFRAMES;
	fps = 0.0;

}

void FrameRate :: Tick ()
{

    if ( numframesremaining == 0 ) {

		int endtime = (int) EclGetAccurateTime ();
		int time = endtime - starttime;
		//fps = (float) NUMFRAMES / (float) time;
		fps = 1000.0f * (float) NUMFRAMES / (float) time;

		numframesremaining = NUMFRAMES;
		starttime = (int) EclGetAccurateTime ();

    }
    else

	--numframesremaining;

}

float FrameRate :: FPS ()
{

    return fps;

}


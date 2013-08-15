
#include "stdio.h"

#include "animation.h"

#include "mmgr.h"

Animation::Animation()
    : buttonname(0), button(0), targetC(0)
{
}

Animation::~Animation()
{

	if ( buttonname )
		delete [] buttonname;
	if ( targetC )
		delete [] targetC;

}

void Animation::DebugPrint ()
{

	printf ( "ANIMATION : buttonname:'%s'\n", buttonname );
	printf ( "			  sX:%d, sY:%d, tX:%d, tY:%d, dX:%f, dY:%f\n", sourceX, sourceY, targetX, targetY, dX, dY );
	printf ( "		      sW:%d, sH:%d, tW:%d, tH:%d, dH:%f, dW:%f\n", sourceW, sourceH, targetW, targetH, dH, dW );
	printf ( "		      target caption : '%s', dC:%f\n", targetC, dC );
	printf ( "			  time:%d, starttime:%d, finishtime:%d, MOVETYPE:%d\n", time, starttime, finishtime, MOVETYPE );

}


// View.cpp: implementation of the View class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h>

#include "app/app.h"
#include "app/globals.h"

#include "options/options.h"

#include "view/view.h"

#include "mmgr.h"

View::View()
{
	
}

View::~View()
{
	
}

void View::Initialise ()
{

	fps.Start ();

}

void View::Draw ()
{

	fps.Tick ();

}


bool View::Load ( FILE *file )
{

	LoadID ( file );
	LoadID_END ( file );

	Initialise ();

	return true;

}

void View::Save ( FILE *file )
{

	SaveID ( file );
	SaveID_END ( file );

}

void View::Print ()
{

	printf ( "============== V I E W =====================================\n" );
	printf ( "============== E N D  O F  V I E W =========================\n" );

}

char *View::GetID ()
{

	return "VIEW";

}

void View::Update ()
{

}


/*

  View class object

	Creates a 3D representation of the game world
	based on the players current location in that world
	Uses OpenGL

  */


#ifndef _included_view_h
#define _included_view_h

// ============================================================================

#include <stdio.h>

#include "app/uplinkobject.h"
#include "view/fps.h"

// ============================================================================


class View : public UplinkObject
{

public:

	FrameRate fps;

public:


	View();
	virtual ~View();

	void Initialise ();
	void Draw ();					// This is the big one


	// Common functions

	bool Load   ( FILE *file );
	void Save   ( FILE *file );
	void Print  ();
	void Update ();	
	
	char *GetID ();

};


#endif 

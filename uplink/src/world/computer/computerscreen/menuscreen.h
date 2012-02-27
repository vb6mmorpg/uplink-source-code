
/*

  MenuScreen class object

  Represents a computer menu screen, with a list of possible options
  and the action to take when those options are selected

  */


#ifndef _included_menuscreen_h
#define _included_menuscreen_h

// ============================================================================

#include "app/uplinkobject.h"

#include "world/computer/computerscreen/computerscreen.h"


class MenuScreenOption;

// ============================================================================

class MenuScreen : public ComputerScreen  
{

protected:

	LList <MenuScreenOption *> options;	

public:

	MenuScreen();
	virtual ~MenuScreen();

	void  AddOption	  ( char *caption, char *tooltip, int nextpage, int security = 10, int index = -1 );
	int   NumOptions  ();
	char *GetCaption  ( int index );
	char *GetTooltip  ( int index );
	int   GetNextPage ( int index );
	int   GetSecurity ( int index );
 

	// Common functions

	bool Load  ( FILE *file );			
	void Save  ( FILE *file );			
	void Print ();						
	
	char *GetID ();						
	int   GetOBJECTID ();				

};


// ============================================================================

#define SIZE_MENUSCREENOPTION_CAPTION   64
#define SIZE_MENUSCREENOPTION_TOOLTIP   128


class MenuScreenOption : public UplinkObject
{

public :

	char caption [SIZE_MENUSCREENOPTION_CAPTION];
	char tooltip [SIZE_MENUSCREENOPTION_TOOLTIP];
	int nextpage;
	int security;

public:

	MenuScreenOption ();
	~MenuScreenOption ();

	void SetCaption ( char *newcaption );
	void SetTooltip ( char *newtooltip );
	void SetNextPage ( int newnextpage );
	void SetSecurity ( int newsecurity );


	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	void Update ();

	char *GetID ();
	int GetOBJECTID ();
	
};


#endif

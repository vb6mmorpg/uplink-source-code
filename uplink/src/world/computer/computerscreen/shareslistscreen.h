
/*

  Shares List Screen

	A specific type of computer screen,
	designed to show a list of shares.

	The list might be the players shares, another persons shares,
	all shares, or the results of a search for a share name.

  */


#ifndef _included_shareslistscreen_h
#define _included_shareslistscreen_h

// ============================================================================

#include "world/computer/computerscreen/computerscreen.h"

#define SHARESLISTSCREENTYPE_NONE				0
#define SHARESLISTSCREENTYPE_ALLSHARES			1
#define SHARESLISTSCREENTYPE_USERSHARES			2
#define SHARESLISTSCREENTYPE_SPECIFICSHARES		3

// ============================================================================



class SharesListScreen : public ComputerScreen
{

public:

	int nextpage;
	int viewpage;	
	int SCREENTYPE;

public:

	SharesListScreen ();
	~SharesListScreen ();

	void SetNextPage ( int newnextpage ); 
	void SetViewPage ( int newviewpage );
	void SetScreenType ( int newSCREEN_TYPE );


	// Common functions

	bool Load  ( FILE *file );			
	void Save  ( FILE *file );			
	void Print ();						
	
	char *GetID ();						
	int   GetOBJECTID ();		

};



#endif


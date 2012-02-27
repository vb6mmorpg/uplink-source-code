
/*

  Cypher Screen Interface

	Asks the user to input a great big number
	which can only really be done with software

  */


#ifndef _included_cypherscreeninterface_h
#define _included_cypherscreeninterface_h


// ==============================================================

#include "world/computer/computerscreen/cypherscreen.h"

#include "interface/remoteinterface/remoteinterfacescreen.h"

#define CYPHER_WIDTH	30
#define CYPHER_HEIGHT	14

// ==============================================================




class CypherScreenInterface : public RemoteInterfaceScreen
{

protected:

	char cypher [CYPHER_WIDTH][CYPHER_HEIGHT];
	bool cypherlock [CYPHER_WIDTH][CYPHER_HEIGHT];
	int numlocked;
	int lastupdate;

protected:

	static void DrawCypher ( Button *button, bool highlighted, bool clicked );
	static void ClickCypher ( Button *button );

	static void BypassClick ( Button *button );
	static void ProceedClick ( Button *button );

public:

	CypherScreenInterface ();
	~CypherScreenInterface ();

	int NumLocked ();								// Once they are all locked,
	int NumUnLocked ();								// The screen can be bypassed
	bool IsLocked ( int x, int y );
	void CypherLock ( int x, int y );
	void CypherLock ();								// Picks an unlocked number at random and locks it
	void Cycle ();

	bool ReturnKeyPressed ();

	void Create ( ComputerScreen *newcs );
	void Remove ();
	bool IsVisible ();
	void Update ();

	int  ScreenID ();			

	CypherScreen *GetComputerScreen ();				// Should override this with correct data type

};


#endif

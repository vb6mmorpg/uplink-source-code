
/*

  Interface for Global Criminal Database

  */


#ifndef _included_criminalscreeninterface_h
#define _included_criminalscreeninterface_h

// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class GenericScreen;

// ============================================================================



class CriminalScreenInterface : public RemoteInterfaceScreen
{

protected:

	char *searchname;
	int recordindex;
	int lastupdate;

protected:

	void UpdateScreen ();									// Uses recordindex

	static void NameDraw        ( Button *button, bool highlighted, bool clicked );
    static void NameBorderDraw  ( Button *button, bool highlighted, bool clicked );
    static void HistoryDraw     ( Button *button, bool highlighted, bool clicked );
    
	static void CloseClick			( Button *button );
	static void AddConvictionClick	( Button *button );
	static void ClearClick			( Button *button );
	static void ArrestClick			( Button *button );

public:

	CriminalScreenInterface ();
	~CriminalScreenInterface ();

    bool EscapeKeyPressed ();

	void SetSearchName ( char *newsearchname );

	void Create ( ComputerScreen *cs );
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

	GenericScreen *GetComputerScreen ();

};



#endif




#ifndef _included_shareslistscreeninterface_h
#define _included_shareslistscreeninterface_h


// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class ComputerScreen;
class SharesListScreen;

// ============================================================================


class SharesListScreenInterface : public RemoteInterfaceScreen
{

protected:

	static int baseoffset;

	LList <char *> fulllist;												// These are lists of company names
	LList <char *> filteredlist;

protected:

	static void ShareDraw ( Button *button, bool highlighted, bool clicked );
	static void ShareClick  ( Button *button );
	static void ShareMouseDown ( Button *button );
	static void ShareMouseMove ( Button *button );

	static void FilterDraw ( Button *button, bool highlighted, bool clicked );
	static void FilterClick ( Button *button );

	static void ScrollUpClick ( Button *button );
	static void ScrollDownClick ( Button *button );

	static void CloseClick ( Button *button );

public:

	void SetFullList ( LList <char *> *newfulllist );						// The list is copied
	void SetFullList ();													// Uses current filtered list
	void ApplyFilter ( char *filter );

    bool ReturnKeyPressed ();

	void Create ();
	void Create ( ComputerScreen *newcs );
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

	SharesListScreen *GetComputerScreen ();

};


#endif

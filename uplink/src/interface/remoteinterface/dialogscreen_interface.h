
/*

  Definition of a Dialog Screen Interface

  */

#ifndef _included_dialogscreeninterface_h
#define _included_dialogscreeninterface_h


// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class ComputerScreen;
class DialogScreen;
class DialogScreenWidget;

// ============================================================================


class DialogScreenInterface : public RemoteInterfaceScreen  
{

protected:

	static void PasswordBoxDraw		( Button *button, bool highlighted, bool clicked );

	static void NextPageClick		( Button *button );
	static void ScriptButtonClick	( Button *button );
	
public:

	DialogScreenInterface();
	virtual ~DialogScreenInterface();

	bool ReturnKeyPressed ();
    bool EscapeKeyPressed ();

	void Create ( ComputerScreen *newcs );
	void Remove ();
	bool IsVisible ();

	int  ScreenID ();			

	DialogScreen *GetComputerScreen ();				// Should override this with correct data type

	static void RemoveWidget ( DialogScreenWidget *dsw, Computer *comp );

};

#endif

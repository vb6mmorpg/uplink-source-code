
/*

  LocalInterfaceScreen object

	A slightly more customized version of Interface Screen

  */

#ifndef _included_localinterface_screen
#define _included_localinterface_screen


#include "eclipse.h"
#include "interface/interfacescreen.h"


class LocalInterfaceScreen : public InterfaceScreen  
{

protected:

	static void BackgroundDraw ( Button *button, bool highlighted, bool clicked );

public:

	LocalInterfaceScreen();
	virtual ~LocalInterfaceScreen();

	virtual void Create ();
	void CreateHeight ( int panelheight );
	virtual void Remove ();
	virtual void Update ();
	virtual bool IsVisible ();

	virtual int  ScreenID ();			

	static LocalInterfaceScreen *GetInterfaceScreen ( int screenID );						// Returns this object, asserts screenID

};

#endif

// StatusInterface.h: interface for the StatusInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _included_statusinterface_h
#define _included_statusinterface_h


#include "interface/localinterface/localinterfacescreen.h"


class StatusInterface : public LocalInterfaceScreen  
{

protected:

	static void MiniTitleDraw ( Button *button, bool highlighted, bool clicked );

	static void TitleClick ( Button *button );

public:

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

};

#endif 

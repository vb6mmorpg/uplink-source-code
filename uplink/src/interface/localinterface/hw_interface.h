/*
	
	  Hardware
	  Interface definition

  */

#ifndef _included_hardwareinterface_h
#define _included_hardwareinterface_h


#include "interface/localinterface/localinterfacescreen.h"


class HWInterface : public LocalInterfaceScreen
{

protected:

	static void TitleClick ( Button *button );
	
	static void ShowGatewayClick ( Button *button );

	static void MiniTitleDraw ( Button *button, bool highlighted, bool clicked );

	static void CreateHWInterface ();
	static void RemoveHWInterface ();
	static bool IsVisibleHWInterface ();

	static void BackClick ( Button *button );
	
public:

	void Create ();
	void Update ();
	void Remove ();
	bool IsVisible ();

	int ScreenID ();

};

#endif



/*

	Interface definition for cheat codes 

  */

#ifndef _included_keymapperinterface_h
#define _included_keymapperinterface_h


#include "interface/localinterface/localinterfacescreen.h"


class KeyMapperInterface : public LocalInterfaceScreen  
{

public:

	static void TitleClick  ( Button *button );
	static void CommitClick ( Button *button );

	static void FunctionClick ( Button *button );
	static void Page1Click ( Button *button );
	static void Page2Click ( Button *button );

	static void Create_FunctionInterface ();
	static void Create_Page1Interface ();
	static void Create_Page2Interface ();

public:

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

};

#endif 

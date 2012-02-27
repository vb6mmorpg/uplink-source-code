
/*

  Brings up a keyboard on screen
  The player can use it to enter text

	*/


#ifndef _included_keyboard_h
#define _included_keyboard_h


#include "interface/localinterface/localinterfacescreen.h"


class KeyboardInterface : public LocalInterfaceScreen  
{

protected:

	static char *targetbuttonname;

protected:

	static void RemoveKeyboardInterface ( Button *button );
	static void Keypress ( Button *button );
	
public:

	KeyboardInterface();
	virtual ~KeyboardInterface();

	void SetTargetButton ( char *bname );

	void Create ( int x, int y );
	void Remove ();
	bool IsVisible ();

};

#endif

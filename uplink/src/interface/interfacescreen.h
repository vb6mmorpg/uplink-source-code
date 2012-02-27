
/*

  InterfaceScreen class object

	Part of the Interface sub-system
	Base class for defining a screen that will be in the interface
	 for example the InRealLife screen, or a password screen etc

  */


#ifndef _included_interfacescreen_h
#define _included_interfacescreen_h

#include "app/uplinkobject.h"

#include "tosser.h"

class InterfaceScreen : public UplinkObject
{

protected:
	DArray<char *> *interface_buttons;
	virtual void RegisterButton ( int x, int y, int width, int height,
								  char *caption, char *name );
	virtual void RegisterButton ( int x, int y, int width, int height,
								  char *caption, char *tooltip, char *name );

public:

	InterfaceScreen();
	virtual ~InterfaceScreen();

	virtual void Create ();
	virtual void Remove ();
	virtual void Update ();
	virtual bool IsVisible ();

	virtual int  ScreenID ();			

};


#endif 


/*

  Company Info screen interface

	Shows basic data on a company
	including size, growth rate, 
	key personel etc

  */


#ifndef _includedcompanyinfoscreeninterface_h
#define _includedcompanyinfoscreeninterface_h


#include "interface/remoteinterface/remoteinterfacescreen.h"

class GenericScreen;




class CompanyInfoScreenInterface : public RemoteInterfaceScreen 
{

protected:

	static void MdAddClick ( Button *button );
	static void AdminAddClick ( Button *button );
    static void BackClick ( Button *button );

public:

	CompanyInfoScreenInterface ();
	~CompanyInfoScreenInterface ();

    bool EscapeKeyPressed ();

	void Create ();
	void Create ( ComputerScreen *newcs );
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

	GenericScreen *GetComputerScreen ();

};


#endif


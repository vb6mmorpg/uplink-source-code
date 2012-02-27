
/*

  Interface for International academic databasae

  */


#ifndef _included_academicscreeninterface_h
#define _included_academicscreeninterface_h

// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class GenericScreen;

// ============================================================================



class AcademicScreenInterface : public RemoteInterfaceScreen
{

protected:

	char *searchname;
	int recordindex;
	int lastupdate;

protected:

	void UpdateScreen ();									// Uses recordindex

	static void DetailsDraw ( Button *button, bool highlighted, bool clicked );

	static void CommitClick			( Button *button );
	static void CloseClick			( Button *button );

public:

	AcademicScreenInterface ();
	~AcademicScreenInterface ();

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


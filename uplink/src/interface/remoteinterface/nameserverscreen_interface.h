
/*

  Interface for International academic databasae

  */


#ifndef _included_nameserverscreeninterface_h
#define _included_nameserverscreeninterface_h

// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class GenericScreen;

// ============================================================================



class NameServerScreenInterface : public RemoteInterfaceScreen
{

protected:

	char *searchname;
	int recordindex;
	int lastupdate;

protected:

	void UpdateScreen ();									// Uses recordindex
	void UpdateLinks ();

	static void DetailsDraw ( Button *button, bool highlighted, bool clicked );
	static void AddLinkDraw ( Button *button, bool highlighted, bool clicked );

	static void CommitClick			( Button *button );
	static void CloseClick			( Button *button );
	static void IPClick				( Button *button );

	static Image *iadd_tif;
	static Image *iadd_h_tif;
	static Image *iadd_c_tif;

public:

	NameServerScreenInterface ();
	~NameServerScreenInterface ();

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


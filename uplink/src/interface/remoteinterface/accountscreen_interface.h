
#ifndef _included_accountscreen_h
#define _included_accountscreen_h

// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class ComputerScreen;
class GenericScreen;
class BankAccount;

// ============================================================================


class AccountScreenInterface : public RemoteInterfaceScreen
{

protected:

	BankAccount *account;
	time_t lastupdate;

protected:

	static void CloseClick ( Button *button );

public:

	AccountScreenInterface ();
	~AccountScreenInterface ();

    bool EscapeKeyPressed ();

	void Create ();
	void Create ( ComputerScreen *cs );
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

	GenericScreen *GetComputerScreen ();				// Should override this with correct data type

};


#endif


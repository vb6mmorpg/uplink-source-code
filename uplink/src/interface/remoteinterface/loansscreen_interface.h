
/*

  Loans Screen Interface

	Appears on public bank servers - allows
	the player to take out new loans, pay off 
	existing loans etc

  */


#ifndef _included_loansscreeninterface_h
#define _included_loansscreeninterface_h


#include "world/computer/bankaccount.h"
#include "world/computer/computerscreen/genericscreen.h"

#include "interface/remoteinterface/remoteinterface.h"


class LoansScreenInterface : public RemoteInterfaceScreen 
{

protected:

	static void IncreaseLoanClick ( Button *button );
	static void DecreaseLoanClick ( Button *button );

	static void CloseClick ( Button *button );

protected:

	BankAccount *account;
	int loanchange;

public:

	LoansScreenInterface ();
	~LoansScreenInterface ();

    bool EscapeKeyPressed ();

	void Create ();
	void Create ( ComputerScreen *cs );
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

	GenericScreen *GetComputerScreen ();			

};


#endif


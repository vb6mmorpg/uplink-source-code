// FinanceInterface.h: interface for the FinanceInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _included_financeinterface_h
#define _included_financeinterface_h


#include "interface/localinterface/localinterfacescreen.h"


class FinanceInterface : public LocalInterfaceScreen  
{

protected:

	int oldbalance;
	int previousnumaccounts;
	time_t lastupdate;

protected:

	static void TitleClick ( Button *button );

	static void DrawAccountsTitle ( Button *button, bool highlighted, bool clicked );

	static void ClickAccountButton ( Button *button );
	static void DrawAccountButton ( Button *button, bool highlighted, bool clicked );

public:

	static void AfterPhoneDialler ( char *ip, char *info );

	FinanceInterface ();
	~FinanceInterface ();

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

};

#endif 

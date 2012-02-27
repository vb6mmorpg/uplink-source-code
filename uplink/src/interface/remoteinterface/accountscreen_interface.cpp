
#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/serialise.h"

#include "game/game.h"
#include "game/data/data.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/accountscreen_interface.h"

#include "world/computer/computer.h"
#include "world/computer/bankcomputer.h"
#include "world/computer/computerscreen/genericscreen.h"

#include "mmgr.h"


AccountScreenInterface::AccountScreenInterface ()
{

	account = NULL;

}

AccountScreenInterface::~AccountScreenInterface ()
{

}

bool AccountScreenInterface::EscapeKeyPressed ()
{

    CloseClick (NULL);
    return true;

}

void AccountScreenInterface::CloseClick ( Button *button )
{

    GenericScreen *gs = (GenericScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
    
	if ( gs->nextpage != -1 ) 
        game->GetInterface ()->GetRemoteInterface ()->RunScreen ( gs->nextpage, gs->GetComputer () );

}

void AccountScreenInterface::Create ()
{

	if ( cs ) Create ( cs );
	else printf ( "AccountScreenInterface::Create, tried to create when GenericScreen==NULL\n" );

}

void AccountScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert (newcs);
	cs = newcs;

	// Look up the BankAccount object and store it
	Computer *comp = cs->GetComputer ();
	UplinkAssert (comp->GetOBJECTID () == OID_BANKCOMPUTER );
	BankComputer *bank = (BankComputer *) comp;

	// Look up the account number based on the log in name
	Record *rec = bank->recordbank.GetRecordFromName ( game->GetInterface ()->GetRemoteInterface ()->security_name );
	if ( !rec ) {
		account = NULL;
		return;
	}

	char *accno = rec->GetField ( RECORDBANK_ACCNO );
	if ( !accno ) {
		account = NULL;
		return;
	}

	// Now look up the account based on that account number
	account = bank->accounts.GetData ( accno );


	if ( !IsVisible () ) {

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "accountscreen_maintitle" );
		EclRegisterButtonCallbacks ( "accountscreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "accountscreen_subtitle" );
		EclRegisterButtonCallbacks ( "accountscreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

		// Account Name

		EclRegisterButton ( 70, 150, 100, 15, "Account Name", "Shows the name your account is in", "accountscreen_accountname_title" );
		EclRegisterButton ( 190, 150, 200, 15, "Accessing...", "", "accountscreen_accountname" );
		EclRegisterButtonCallbacks ( "accountscreen_accountname", textbutton_draw, NULL, NULL, NULL );

		// Account number

		EclRegisterButton ( 70, 175, 100, 15, "Account No.", "Shows your account number", "accountscreen_accountnumber_title" );
		EclRegisterButton ( 190, 175, 100, 15, "Accessing...", "", "accountscreen_accountnumber" );
		EclRegisterButtonCallbacks ( "accountscreen_accountnumber", textbutton_draw, NULL, NULL, NULL );

		// Balance

		EclRegisterButton ( 70, 200, 100, 15, "Balance", "Shows your bank balance", "accountscreen_balance_title" );
		EclRegisterButton ( 190, 200, 100, 15, "Accessing...", "", "accountscreen_balance" );
		EclRegisterButtonCallbacks ( "accountscreen_balance", textbutton_draw, NULL, NULL, NULL );

		// Loan

		EclRegisterButton ( 70, 225, 100, 15, "Loan", "Shows your the size of your loan", "accountscreen_loan_title" );
		EclRegisterButton ( 190, 225, 100, 15, "Accessing...", "", "accountscreen_loan" );
		EclRegisterButtonCallbacks ( "accountscreen_loan", textbutton_draw, NULL, NULL, NULL );

		// Interest rate

		EclRegisterButton ( 300, 225, 80, 15, "", "", "accountscreen_loanrate" );
		EclRegisterButtonCallbacks ( "accountscreen_loanrate", textbutton_draw, NULL, NULL, NULL );


		// Put text in any fields that are constant

		if ( account ) {

			// Account name
			EclGetButton ( "accountscreen_accountname" )->SetCaption ( account->name );

			// Account number
			char accno [16];
			UplinkSnprintf ( accno, sizeof ( accno ), "%d", account->accountnumber );
			EclGetButton ( "accountscreen_accountnumber" )->SetCaption ( accno );


		}
		else {

			EclGetButton ( "accountscreen_accountname" )->SetCaption ( "Unknown" );
			EclGetButton ( "accountscreen_accountnumber" )->SetCaption ( "Unknown" );

		}

		// OK button

		EclRegisterButton ( 150, 300, 100, 15, "OK", "Return to the account management screen", "accountscreen_close" );
		EclRegisterButtonCallback ( "accountscreen_close", CloseClick ); 

	}

	lastupdate = 0;
	Update ();

}

void AccountScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "accountscreen_maintitle" );
		EclRemoveButton ( "accountscreen_subtitle" );

		EclRemoveButton ( "accountscreen_accountname_title" );
		EclRemoveButton ( "accountscreen_accountname" );

		EclRemoveButton ( "accountscreen_accountnumber_title" );
		EclRemoveButton ( "accountscreen_accountnumber" );

		EclRemoveButton ( "accountscreen_balance_title" );
		EclRemoveButton ( "accountscreen_balance" );

		EclRemoveButton ( "accountscreen_loan_title" );
		EclRemoveButton ( "accountscreen_loan" );
		EclRemoveButton ( "accountscreen_loanrate" );

		EclRemoveButton ( "accountscreen_close" );

	}

}

void AccountScreenInterface::Update ()
{

	// Update every 5 seconds
	// (If there is an account to update)

	if ( time (NULL) > lastupdate + 5 ) {

		if ( account ) {

			// Update the balance

			char balance [16];
			UplinkSnprintf ( balance, sizeof ( balance ), "%d", account->balance );
			EclGetButton ( "accountscreen_balance" )->SetCaption ( balance );
		
			// Update the loan

			char loan [16];
			UplinkSnprintf ( loan, sizeof ( loan ), "%d", account->loan );
			EclGetButton ( "accountscreen_loan" )->SetCaption ( loan );

			// Update the interest rate 

			char rate [16];
			if        ( account->loan <= SMALLLOAN_MAX ) {		UplinkSnprintf ( rate, sizeof ( rate ), "(%d%% apr)", int (100.0 * SMALLLOAN_INTEREST) );
			} else if ( account->loan <= MEDIUMLOAN_MAX ) {		UplinkSnprintf ( rate, sizeof ( rate ), "(%d%% apr)", int (100.0 * MEDIUMLOAN_INTEREST) );
			} else if ( account->loan <= LARGELOAN_MAX ) {		UplinkSnprintf ( rate, sizeof ( rate ), "(%d%% apr)", int (100.0 * LARGELOAN_INTEREST) );
			} else {											UplinkSnprintf ( rate, sizeof ( rate ), "(%d%% apr)", int (100.0 * MAXLOAN_INTEREST) );
			}

			EclGetButton ( "accountscreen_loanrate" )->SetCaption ( rate );

		}

		lastupdate = time (NULL);

	}
	
}

bool AccountScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "accountscreen_maintitle" ) != 0 );

}

int AccountScreenInterface::ScreenID ()
{

	return SCREEN_ACCOUNTSCREEN;

}

GenericScreen *AccountScreenInterface::GetComputerScreen ()
{

	UplinkAssert ( cs );
	return ( GenericScreen * ) cs;

}

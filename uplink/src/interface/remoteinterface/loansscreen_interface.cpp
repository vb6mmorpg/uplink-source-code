
#include "eclipse.h"


#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"
#include "app/opengl_interface.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/bankcomputer.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/loansscreen_interface.h"

#include "mmgr.h"



LoansScreenInterface::LoansScreenInterface ()
{
	
	account = NULL;
	loanchange = 0;

}

LoansScreenInterface::~LoansScreenInterface ()
{
}

bool LoansScreenInterface::EscapeKeyPressed ()
{

    CloseClick (NULL);
    return true;

}

void LoansScreenInterface::IncreaseLoanClick ( Button *button )
{

	LoansScreenInterface *thisint = (LoansScreenInterface *) GetInterfaceScreen ( SCREEN_LOANSSCREEN );
	UplinkAssert (thisint);

	Person *accountowner;
	if ( thisint->account && thisint->IsVisible () && ( accountowner = thisint->account->GetPerson () ) ) {
		
		// Make sure we don't take out too much

		int maxcredit = thisint->account->loan + accountowner->rating.creditrating * 100;

		if ( thisint->account->loan < maxcredit ) {

			thisint->loanchange += 100;
			if ( thisint->account->loan + thisint->loanchange > maxcredit ) 
				thisint->loanchange = maxcredit - thisint->account->loan;

			int newloan = thisint->account->loan + thisint->loanchange;
			int newbalance = thisint->account->balance + thisint->loanchange;

			// Update the balance

			char balance [16];
			UplinkSnprintf ( balance, sizeof ( balance ), "%d", newbalance );
			EclGetButton ( "loansscreen_balance" )->SetCaption ( balance );
		
			// Update the loan

			char loan [16];
			UplinkSnprintf ( loan, sizeof ( loan ), "%d", newloan );
			EclGetButton ( "loansscreen_loan" )->SetCaption ( loan );

			// Update the interest rate 

			char rate [16];
			if		( newloan <= SMALLLOAN_MAX ) {			UplinkSnprintf ( rate, sizeof ( rate ), "(%d%% apr)", int (100.0 * SMALLLOAN_INTEREST) );
			} else if ( newloan <= MEDIUMLOAN_MAX ) {		UplinkSnprintf ( rate, sizeof ( rate ), "(%d%% apr)", int (100.0 * MEDIUMLOAN_INTEREST) );
			} else if ( newloan <= LARGELOAN_MAX ) {		UplinkSnprintf ( rate, sizeof ( rate ), "(%d%% apr)", int (100.0 * LARGELOAN_INTEREST) );
			} else {										UplinkSnprintf ( rate, sizeof ( rate ), "(%d%% apr)", int (100.0 * MAXLOAN_INTEREST) );
			}

			EclGetButton ( "loansscreen_loanrate" )->SetCaption ( rate );

		}

	}

}

void LoansScreenInterface::DecreaseLoanClick ( Button *button )
{

	LoansScreenInterface *thisint = (LoansScreenInterface *) GetInterfaceScreen ( SCREEN_LOANSSCREEN );
	UplinkAssert (thisint);

	Person *accountowner;
	if ( thisint->account && thisint->IsVisible () && ( accountowner = thisint->account->GetPerson () ) ) {
		
		thisint->loanchange -= 100;
		if ( thisint->account->loan + thisint->loanchange < 0 ) thisint->loanchange = thisint->account->loan * -1;

		int newloan = thisint->account->loan + thisint->loanchange;
		int newbalance = thisint->account->balance + thisint->loanchange;

		// Update the balance

		char balance [16];
		UplinkSnprintf ( balance, sizeof ( balance ), "%d", newbalance );
		EclGetButton ( "loansscreen_balance" )->SetCaption ( balance );
	
		// Update the loan

		char loan [16];
		UplinkSnprintf ( loan, sizeof ( loan ), "%d", newloan );
		EclGetButton ( "loansscreen_loan" )->SetCaption ( loan );

		// Update the interest rate 

		char rate [16];
		if		( newloan <= SMALLLOAN_MAX ) {			UplinkSnprintf ( rate, sizeof ( rate ), "(%d%% apr)", int (100.0 * SMALLLOAN_INTEREST) );
		} else if ( newloan <= MEDIUMLOAN_MAX ) {		UplinkSnprintf ( rate, sizeof ( rate ), "(%d%% apr)", int (100.0 * MEDIUMLOAN_INTEREST) );
		} else if ( newloan <= LARGELOAN_MAX ) {		UplinkSnprintf ( rate, sizeof ( rate ), "(%d%% apr)", int (100.0 * LARGELOAN_INTEREST) );
		} else {										UplinkSnprintf ( rate, sizeof ( rate ), "(%d%% apr)", int (100.0 * MAXLOAN_INTEREST) );
		}

		EclGetButton ( "loansscreen_loanrate" )->SetCaption ( rate );
		
	}

}

void LoansScreenInterface::CloseClick ( Button *button )
{

	// Update the account if changed

	LoansScreenInterface *thisint = (LoansScreenInterface *) GetInterfaceScreen ( SCREEN_LOANSSCREEN );
	UplinkAssert (thisint);

	Person *accountowner;
	if ( thisint->account && thisint->loanchange != 0 && ( accountowner = thisint->account->GetPerson () ) ) {

		thisint->account->loan += thisint->loanchange;
		thisint->account->balance += thisint->loanchange;

        int creditchange = thisint->loanchange / 100;
        accountowner->rating.ChangeCreditRating ( creditchange *-1 );

	}

	// Go to the next page

	int nextpage = thisint->GetComputerScreen ()->nextpage;
	if ( nextpage != -1 ) 
        game->GetInterface ()->GetRemoteInterface ()->RunScreen ( nextpage, thisint->GetComputerScreen ()->GetComputer () );

}

void LoansScreenInterface::Create ()
{

	if ( cs ) Create ( cs );
	else printf ( "LoansScreenInterface::Create, tried to create when GenericScreen==NULL\n" );

}

void LoansScreenInterface::Create ( ComputerScreen *newcs )
{

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

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "loansscreen_maintitle" );
		EclRegisterButtonCallbacks ( "loansscreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "loansscreen_subtitle" );
		EclRegisterButtonCallbacks ( "loansscreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

		// Account Name

		EclRegisterButton ( 70, 150, 100, 15, "Account Name", "Shows the name your account is in", "loansscreen_accountname_title" );
		EclRegisterButton ( 190, 150, 100, 15, "Accessing...", "", "loansscreen_accountname" );
		EclRegisterButtonCallbacks ( "loansscreen_accountname", textbutton_draw, NULL, NULL, NULL );

		// Account number

		EclRegisterButton ( 70, 175, 100, 15, "Account No.", "Shows your account number", "loansscreen_accountnumber_title" );
		EclRegisterButton ( 190, 175, 100, 15, "Accessing...", "", "loansscreen_accountnumber" );
		EclRegisterButtonCallbacks ( "loansscreen_accountnumber", textbutton_draw, NULL, NULL, NULL );

		// Balance

		EclRegisterButton ( 70, 200, 100, 15, "Balance", "Shows your bank balance", "loansscreen_balance_title" );
		EclRegisterButton ( 190, 200, 100, 15, "Accessing...", "", "loansscreen_balance" );
		EclRegisterButtonCallbacks ( "loansscreen_balance", textbutton_draw, NULL, NULL, NULL );

		// Loan

		EclRegisterButton ( 70, 225, 100, 15, "Loan", "Shows your the size of your loan", "loansscreen_loan_title" );
		EclRegisterButton ( 190, 225, 100, 15, "Accessing...", "", "loansscreen_loan" );
		EclRegisterButtonCallbacks ( "loansscreen_loan", textbutton_draw, NULL, NULL, NULL );

		// Interest rate

		EclRegisterButton ( 300, 225, 80, 15, "", "", "loansscreen_loanrate" );
		EclRegisterButtonCallbacks ( "loansscreen_loanrate", textbutton_draw, NULL, NULL, NULL );


		if ( account ) {

			// Account name

			EclGetButton ( "loansscreen_accountname" )->SetCaption ( account->name );

			// Account number

			char accno [16];
			UplinkSnprintf ( accno, sizeof ( accno ), "%d", account->accountnumber );
			EclGetButton ( "loansscreen_accountnumber" )->SetCaption ( accno );

			// Update the balance

			char balance [16];
			UplinkSnprintf ( balance, sizeof ( balance ), "%d", account->balance );
			EclGetButton ( "loansscreen_balance" )->SetCaption ( balance );
		
			// Update the loan

			char loan [16];
			UplinkSnprintf ( loan, sizeof ( loan ), "%d", account->loan );
			EclGetButton ( "loansscreen_loan" )->SetCaption ( loan );

			// Update the interest rate 

			char rate [16];
			if		( account->loan <= SMALLLOAN_MAX ) {		UplinkSnprintf ( rate, sizeof ( rate ), "(%d%% apr)", int (100.0 * SMALLLOAN_INTEREST) );
			} else if ( account->loan <= MEDIUMLOAN_MAX ) {		UplinkSnprintf ( rate, sizeof ( rate ), "(%d%% apr)", int (100.0 * MEDIUMLOAN_INTEREST) );
			} else if ( account->loan <= LARGELOAN_MAX ) {		UplinkSnprintf ( rate, sizeof ( rate ), "(%d%% apr)", int (100.0 * LARGELOAN_INTEREST) );
			} else {											UplinkSnprintf ( rate, sizeof ( rate ), "(%d%% apr)", int (100.0 * MAXLOAN_INTEREST) );
			}

			EclGetButton ( "loansscreen_loanrate" )->SetCaption ( rate );

			loanchange = 0;

		}
		else {

			EclGetButton ( "loansscreen_accountname" )->SetCaption ( "Unknown" );
			EclGetButton ( "loansscreen_accountnumber" )->SetCaption ( "Unknown" );

		}

		// Increase loan button

		EclRegisterButton ( 150, 260, 100, 15, "Increase loan", "Click to take out a larger loan", "loansscreen_increaseloan" );
		EclRegisterButtonCallback ( "loansscreen_increaseloan", IncreaseLoanClick );

		// Pay off loan button

		EclRegisterButton ( 150, 280, 100, 15, "Decrease loan", "Click to pay off some of the loan", "loansscreen_decreaseloan" );
		EclRegisterButtonCallback ( "loansscreen_decreaseloan", DecreaseLoanClick );

		// Max available loan button

		char maxloan [32];
		Person *accountowner;
		if ( account && ( accountowner = account->GetPerson () ) ) {
			UplinkSnprintf ( maxloan, sizeof ( maxloan ), "Max loan: %dc", account->loan + accountowner->rating.creditrating * 100 );
		}
		else {
			UplinkStrncpy ( maxloan, "Max loan: 0dc", sizeof ( maxloan ) );
		}

		EclRegisterButton ( 270, 260, 100, 15, maxloan, "", "loansscreen_maxloan" );
		EclRegisterButtonCallbacks ( "loansscreen_maxloan", textbutton_draw, NULL, NULL, NULL );
		
		// OK button

		EclRegisterButton ( 150, 340, 100, 15, "OK", "Return to the account management screen", "loansscreen_close" );
		EclRegisterButtonCallback ( "loansscreen_close", CloseClick ); 


	}

}

void LoansScreenInterface::Remove ()
{
	
	if ( IsVisible () ) {

		EclRemoveButton ( "loansscreen_maintitle" );
		EclRemoveButton ( "loansscreen_subtitle" );

		EclRemoveButton ( "loansscreen_accountname_title" );
		EclRemoveButton ( "loansscreen_accountname" );

		EclRemoveButton ( "loansscreen_accountnumber_title" );
		EclRemoveButton ( "loansscreen_accountnumber" );

		EclRemoveButton ( "loansscreen_balance_title" );
		EclRemoveButton ( "loansscreen_balance" );

		EclRemoveButton ( "loansscreen_loan_title" );
		EclRemoveButton ( "loansscreen_loan" );
		EclRemoveButton ( "loansscreen_loanrate" );

		EclRemoveButton ( "loansscreen_increaseloan" );
		EclRemoveButton ( "loansscreen_decreaseloan" );
		EclRemoveButton ( "loansscreen_maxloan" );

		EclRemoveButton ( "loansscreen_close" );

	}

}

void LoansScreenInterface::Update ()
{
}

bool LoansScreenInterface::IsVisible ()
{
	
	return ( EclGetButton ( "loansscreen_maintitle" ) != NULL );

}

int LoansScreenInterface::ScreenID ()
{

	return SCREEN_LOANSSCREEN;

}


GenericScreen *LoansScreenInterface::GetComputerScreen ()
{

	return (GenericScreen *) cs;

}


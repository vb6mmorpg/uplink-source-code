
#include <strstream>

#include "soundgarden.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"

#include "world/world.h"
#include "world/vlocation.h"
#include "world/generator/consequencegenerator.h"
#include "world/scheduler/transferevent.h"

#include "world/computer/bankcomputer.h"
#include "world/computer/bankaccount.h"

#include "options/options.h"

#include "mmgr.h"



TransferEvent::TransferEvent ()
{

	source = NULL;
	dest = NULL;
	reason = NULL;

	accountfrom = 0;
	accountto = 0;
	amount = 0;

}

TransferEvent::~TransferEvent ()
{

	if ( reason ) delete [] reason;
	if ( source ) delete [] reason;
	if ( dest ) delete [] reason;

}


void TransferEvent::SetFrom( char *sourcecomputer, int sourceaccount )
{

	if ( source )
		delete [] source;
	source = new char [strlen(sourcecomputer)+1];
//	UplinkStrncpy ( source, sourcecomputer, sizeof ( source ) );
	UplinkSafeStrcpy ( source, sourcecomputer );

	accountfrom = sourceaccount;
}

void TransferEvent::SetTo( char *destcomputer, int destaccount )
{

	if ( dest )
		delete [] dest;
	dest = new char [strlen(destcomputer)+1];
//	UplinkStrncpy ( dest, destcomputer, sizeof ( dest ) );
	UplinkSafeStrcpy ( dest, destcomputer );

	accountto = destaccount;
}

void TransferEvent::SetDetails( int transferamount, char *transferreason )
{

	if ( reason )
		delete [] reason;
	reason = new char [strlen(transferreason)+1];
//	UplinkStrncpy ( reason, transferreason, sizeof ( reason ) );
	UplinkSafeStrcpy ( reason, transferreason );

	amount = transferamount;
}


void TransferEvent::Run ()
{

	if ( source == NULL )
	{
		BankComputer *destbank = (BankComputer *)game->GetWorld()->GetComputer(dest);
		UplinkAssert ( destbank );

		char toacc[64];
		UplinkSnprintf(toacc, sizeof(toacc), "%d", accountto);

		BankAccount *destaccount = BankAccount::GetAccount(destbank->ip, toacc);
		UplinkAssert ( destaccount );

		if ( amount < 0 && (abs(amount) > destaccount->balance ) ) { amount = -(destaccount->balance); }

		destaccount->ChangeBalance(amount,reason);
	}
	else if ( dest == NULL )
	{
		BankComputer *sourcebank = (BankComputer *)game->GetWorld()->GetComputer(source);
		UplinkAssert ( sourcebank );

		char fromacc[64];
		UplinkSnprintf(fromacc, sizeof(fromacc), "%d", accountfrom);

		BankAccount *sourceaccount = BankAccount::GetAccount(sourcebank->ip, fromacc);
		UplinkAssert ( sourceaccount );

		if ( amount < 0 && (abs(amount) > sourceaccount->balance ) ) { amount = -(sourceaccount->balance); }

		sourceaccount->ChangeBalance(amount,reason);
	}
	else
	{
		BankComputer *sourcebank = (BankComputer *)game->GetWorld()->GetComputer(source);
		UplinkAssert ( sourcebank );

		BankComputer *destbank = (BankComputer *)game->GetWorld()->GetComputer(dest);
		UplinkAssert ( destbank );

		char fromacc[64];
		UplinkSnprintf(fromacc, sizeof(fromacc), "%d", accountfrom);

		char toacc[64];
		UplinkSnprintf(toacc, sizeof(toacc), "%d", accountto);

		BankAccount *sourceaccount = BankAccount::GetAccount(sourcebank->ip, fromacc);
		UplinkAssert ( sourceaccount );

		amount = min(amount, sourceaccount->balance);

		bool success = BankAccount::TransferMoney( sourcebank->ip, fromacc, destbank->ip, toacc, amount, sourceaccount->GetPerson() );
	}
}

char *TransferEvent::GetShortString ()
{

	size_t shortstringsize = 32;
	char *shortstring = new char [shortstringsize];

	UplinkSnprintf ( shortstring, shortstringsize, "Transfer %d credits", amount );

	return shortstring;
}

char *TransferEvent::GetLongString ()
{

	std::ostrstream longstring;
	longstring << "TransferEvent:\n";
	if ( source ) longstring << "From: " << source << "\nAccount: " << accountfrom << "\n";
	if ( dest ) longstring << "To: " << dest << "\nAccount: " << accountto << "\n";
	if ( reason ) longstring << "Reason: " << reason << "\nAmount: " << amount << "\n";

	longstring << '\x0';

	return longstring.str ();

}


bool TransferEvent::Load  ( FILE *file )
{

	LoadID ( file );

	if ( !UplinkEvent::Load ( file ) ) return false;

	fread ( &accountfrom, sizeof(accountfrom), 1,file );
	fread ( &accountto, sizeof(accountto), 1,file );
	fread ( &amount, sizeof(amount), 1,file );

	if ( !LoadDynamicStringPtr ( &source, file ) ) return false;
	if ( !LoadDynamicStringPtr ( &dest, file ) ) return false;
	if ( !LoadDynamicStringPtr ( &reason, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void TransferEvent::Save  ( FILE *file )
{

	SaveID ( file );

	UplinkEvent::Save ( file );

	fwrite ( &accountfrom, sizeof(accountfrom), 1,file );
	fwrite ( &accountto, sizeof(accountto), 1,file );
	fwrite ( &amount, sizeof(amount), 1,file );

	SaveDynamicString ( source, file );
	SaveDynamicString ( dest, file );
	SaveDynamicString ( reason, file );

	SaveID_END ( file );

}

void TransferEvent::Print ()
{

	printf ( "Transfer Event for %d credits:\n", amount );
	UplinkEvent::Print ();

	if ( source ) printf ( "Source:%s %d\n", source, accountfrom );
	if ( dest ) printf ( "Dest:%s %d\n", dest, accountto );
	if ( reason ) printf ( "Reason:%s\nAmount:%d\n", reason, amount );


}

char *TransferEvent::GetID ()
{

	return "EVT_TRN";

}

int   TransferEvent::GetOBJECTID ()
{

	return OID_TRANSFEREVENT;

}


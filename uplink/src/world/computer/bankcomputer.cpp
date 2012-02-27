
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "world/computer/bankcomputer.h"
#include "world/generator/numbergenerator.h"

#include "mmgr.h"


BankComputer::BankComputer () : Computer ()
{

	previousaccno = NumberGenerator::RandomNumber ( 999999 );

}

BankComputer::~BankComputer ()
{

	DeleteBTreeData ( (BTree <UplinkObject *> *) &accounts );	

}

int BankComputer::GenerateUniqueAccountNumber ()
{

	previousaccno += NumberGenerator::RandomNumber ( 999999 );
	
	char previousaccno_s [12];
	UplinkSnprintf ( previousaccno_s, sizeof ( previousaccno_s ), "%d", previousaccno );

	while ( accounts.LookupTree ( previousaccno_s ) ) {

		previousaccno += NumberGenerator::RandomNumber ( 999999 );	
		UplinkSnprintf ( previousaccno_s, sizeof ( previousaccno_s ), "%d", previousaccno );

	}

	return previousaccno;

}

int BankComputer::CreateBankAccount ( BankAccount *newaccount )
{

	UplinkAssert ( newaccount );

	if ( newaccount->accountnumber == 0 )
		printf ( "BankComputer::CreateBankAccount WARNING : AccountNumber = 0\n" );

	char s_account [16];
	UplinkSnprintf ( s_account, sizeof ( s_account ), "%d", newaccount->accountnumber );

	UplinkAssert ( !(accounts.LookupTree ( s_account )) );

	accounts.PutData ( s_account, newaccount );

	// Create the access record

	Record *rec = new Record ();
	//Due to buggy code all around Uplink, don't change: rec->AddField ( RECORDBANK_NAME, newaccount->name );
	//rec->AddField ( "Name", newaccount->name );
	rec->AddField ( RECORDBANK_NAME, newaccount->accountnumber );
	rec->AddField ( RECORDBANK_PASSWORD, newaccount->password );
	rec->AddField ( RECORDBANK_SECURITY, newaccount->security );
	rec->AddField ( RECORDBANK_ACCNO, newaccount->accountnumber );
	recordbank.AddRecordSorted ( rec );

	return newaccount->accountnumber;

}

int BankComputer::CreateBankAccount ( char *name, char *password, int security,
										int balance, int loan )
{

	BankAccount *ba = new BankAccount ();
	ba->SetOwner ( name );
	ba->SetSecurity ( password, security );
	ba->SetBalance ( balance, loan );
	ba->SetAccNumber ( GenerateUniqueAccountNumber () );

	return CreateBankAccount ( ba );

}

void BankComputer::CloseAccount ( int accno )
{

    char s_accno [64];
    UplinkSnprintf ( s_accno, sizeof ( s_accno ), "%d", accno );

    // TODO 

}


BankAccount *BankComputer::GetRandomAccount ()
{

	// Put the accounts into a DArray then choose a random one
	DArray <BankAccount *> *tempaccounts = accounts.ConvertToDArray ();

    if ( tempaccounts->Size () == 0 ) {
        delete tempaccounts;
        return NULL;
    }

	int index = NumberGenerator::RandomNumber ( tempaccounts->Size () );
	UplinkAssert (tempaccounts->ValidIndex (index) );
	BankAccount *account = tempaccounts->GetData (index);

	delete tempaccounts;
	return account;

}

bool BankComputer::Load ( FILE *file )
{

	LoadID ( file );

	if ( !Computer::Load ( file ) ) return false;

	if ( !FileReadData ( &previousaccno, sizeof(previousaccno), 1, file ) ) return false;

	if ( !LoadBTree ( (BTree <UplinkObject *> *) &accounts, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void BankComputer::Save ( FILE *file )
{

	SaveID ( file );

	Computer::Save ( file );

	fwrite ( &previousaccno, sizeof(previousaccno), 1, file );

	SaveBTree ( (BTree <UplinkObject *> *) &accounts, file );	

	SaveID_END ( file );

}

void BankComputer::Print ()
{

	printf ( "Bank Computer : \n" );
	Computer::Print ();
	printf ( "PreviousAccountNumber = %d\n", previousaccno );
	PrintBTree ( (BTree <UplinkObject *> *) &accounts );	

}

void BankComputer::Update ()
{

	Computer::Update ();

}

char *BankComputer::GetID ()
{

	return "BCOMP";

}

int BankComputer::GetOBJECTID ()
{

	return OID_BANKCOMPUTER;

}


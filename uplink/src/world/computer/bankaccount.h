
/*

  Bank Account class object

	A specific version of Company	
	Part of the World Sub-system

  */


#ifndef _included_bankaccount_h
#define _included_bankaccount_h


#include "app/uplinkobject.h"

#include "world/person.h"
#include "world/computer/logbank.h"

class Person;


class BankAccount : public UplinkObject
{

public:
	
	char name [SIZE_PERSON_NAME];
	char password [64];
	int security;
	int accountnumber;
	int balance;
	int loan;

	LogBank log;

public:

	BankAccount ();
	~BankAccount ();

	void SetOwner		 ( char *newname );
	void SetSecurity	 ( char *newpassword, int newsecurity );
	void SetAccNumber	 ( int newaccnumber );	
	void SetBalance		 ( int newbalance, int newloan );

	void ChangeBalance	 ( int amount, char *description = NULL );	
	
	// Has this account sent money to target account
	// If partial is true,  it return true if there is a log of the transaction on the target _or_ source account
	// If partial is false, it return true if there is a log of the transaction on the target _and_ source account
	bool HasTransferOccured ( char *s_ip, char *t_ip, int t_accno, int amount, bool partial = false );
	bool HasTransferOccured ( char *person, int amount );								// Has this account received payment from person

	Person *GetPerson ();

	static BankAccount *GetAccount ( char *bankip, char *accno );
	static bool	TransferMoney ( char *source_ip, char *source_accno,
								char *target_ip, char *target_accno,
								int amount, Person *person );

	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	
	char *GetID ();
	int   GetOBJECTID ();

};

#endif 

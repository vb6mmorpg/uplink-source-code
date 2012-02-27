
/*

  Company class object
  Part of the World sub-system

	Represents a company in the game world.  The company could be
	a corporation, military, government, academic etc.

  */


#ifndef _included_company_h
#define _included_company_h

// ============================================================================

#include "tosser.h"

#include "app/uplinkobject.h"

#include "world/person.h"

#define COMPANYTYPE_UNKNOWN     0

#define COMPANYTYPE_INDUSTRIAL  1
#define COMPANYTYPE_COMMERCIAL  2
#define COMPANYTYPE_FINANCIAL   3
#define COMPANYTYPE_ACADEMIC    4
#define COMPANYTYPE_GOVERNMENT  6


#define SIZE_COMPANY_NAME   64

// ============================================================================


class Company : public UplinkObject  
{

protected:

	int sharehistory [12];						// indexed on month
	int lastmonthset;							// Most recently set value index in sharehistory

public:

	char name [SIZE_COMPANY_NAME];
	char boss [SIZE_PERSON_NAME];
	char administrator [SIZE_PERSON_NAME];

	int size;
	int TYPE;
	int growth;									// Percentage
	int alignment;								// eg -100 = Satanic ... 0 = Neutral ... +100 = Angelic

public:

	Company();
	virtual ~Company();

	void SetName ( char *newname );	

	void SetSize ( int newsize );
	void SetTYPE ( int newTYPE );
	void SetGrowth ( int newgrowth );
	void SetAlignment ( int newalignment );

	void SetBoss ( char *bossname );
	void SetAdmin ( char *adminname );

	void Grow ( int dayspast );					// Applies growth
	void VaryGrowth ();

	int GetSharePrice ( int month = -1 );		// If -1, returns price of share now
	int GetShareChange ();						// From previous month, in percent
	int GetShareLastMonthSet ();				// Index of recently set share price
	
	// Common functions

	virtual bool Load  ( FILE *file );
	virtual void Save  ( FILE *file );
	virtual void Print ();
	virtual void Update ();
	
	virtual char *GetID ();
	virtual int   GetOBJECTID ();

};


#endif 

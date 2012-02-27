// Company.cpp: implementation of the Company class.
//
//////////////////////////////////////////////////////////////////////

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"

#include "world/world.h"
#include "world/company/company.h"
#include "world/generator/numbergenerator.h"

#include "mmgr.h"


Company::Company()
{

	UplinkStrncpy ( name, " ", sizeof ( name ) );
	UplinkStrncpy ( boss, "Unlisted", sizeof ( boss ) );
	UplinkStrncpy ( administrator, "Unlisted", sizeof ( administrator ) );
	size=0;
	TYPE=0;
	growth=0;
	alignment=0;
	
	for ( int i = 0; i < 12; ++i )
		sharehistory [i] = 0;

	lastmonthset = 0;

}

Company::~Company()
{

}

void Company::SetName ( char *newname )
{

	UplinkAssert ( strlen (newname) < SIZE_COMPANY_NAME );
	UplinkStrncpy ( name, newname, sizeof ( name ) );

}

void Company::SetBoss ( char *bossname )
{

	UplinkAssert ( strlen (bossname) < SIZE_PERSON_NAME );
	UplinkStrncpy ( boss, bossname, sizeof ( boss ) );

}

void Company::SetAdmin ( char *adminname )
{

	UplinkAssert ( strlen(adminname) < SIZE_PERSON_NAME );
	UplinkStrncpy ( administrator, adminname, sizeof ( administrator ) );

}

void Company::SetSize ( int newsize )
{

	size = newsize;

}

void Company::SetTYPE ( int newTYPE )
{

	TYPE = newTYPE;

}

void Company::SetGrowth ( int newgrowth )
{

	growth = newgrowth;

}

void Company::SetAlignment ( int newalignment )
{

	alignment = newalignment;

}

void Company::Grow ( int dayspast )
{

	int oldsize = size;

	float fraction_of_growth = (float) dayspast / 90;						// 90 days is ~ 1/4 year

	size = (int) ( size * ( (100.0 + (fraction_of_growth * growth)) / 100.0 ) );

	// Put our new price into our share history

	int month = game->GetWorld ()->date.GetMonth () - 1;
	UplinkAssert ( month >= 0 && month < 12 );
    sharehistory [month] = NumberGenerator::ApplyVariance ( size, 30 );
    if ( sharehistory [month] < 1 ) sharehistory [month] = 1;
	
	lastmonthset = month;

	// If the previous month share price is 0, then we should 
	// generate previous share prices to fill in the gap

	int index = month - 1;
	if ( index < 0 ) index = 11;

	while ( sharehistory [index] == 0 ) {
		sharehistory [index] = NumberGenerator::RandomNumber ( size );
		index--;
		if ( index < 0 ) index = 11;
	}

}

void Company::VaryGrowth ()
{

	growth = NumberGenerator::ApplyVariance ( growth, 20 );

}

int Company::GetSharePrice ( int month )
{

	if ( month == -1 ) 
		month = lastmonthset;

	UplinkAssert ( month >= 0 && month < 12 );

	return ( sharehistory [ month ] );

}

int Company::GetShareChange ()
{

	int month = lastmonthset;
	UplinkAssert ( month >= 0 && month < 12 );
	int currentprice = sharehistory [month];

	--month;
	if ( month < 0 ) month = 11;
	int oldprice = sharehistory [month];

	if ( oldprice == 0 ) return 0;

	int percentchange = (int)(100 * (float) ( currentprice - oldprice ) / (float) oldprice);
	return percentchange;

}

int Company::GetShareLastMonthSet ()
{

	return lastmonthset;

}

bool Company::Load  ( FILE *file )
{

	LoadID ( file );

	if ( !LoadDynamicStringStatic ( name, SIZE_COMPANY_NAME, file ) ) return false;
	if ( !LoadDynamicStringStatic ( boss, SIZE_PERSON_NAME, file ) ) return false;
	if ( !LoadDynamicStringStatic ( administrator, SIZE_PERSON_NAME, file ) ) return false;

	if ( !FileReadData ( &size, sizeof(size), 1, file ) ) return false;
	if ( !FileReadData ( &TYPE, sizeof(TYPE), 1, file ) ) return false;
	if ( !FileReadData ( &growth, sizeof(growth), 1, file ) ) return false;
	if ( !FileReadData ( &alignment, sizeof(alignment), 1, file ) ) return false;

	for ( int i = 0; i < 12; ++i ) 
		if ( !FileReadData ( &sharehistory [i], sizeof(sharehistory [i]), 1, file ) ) return false;

	if ( !FileReadData ( &lastmonthset, sizeof(lastmonthset), 1, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void Company::Save  ( FILE *file )
{

	SaveID ( file );

	SaveDynamicString ( name, SIZE_COMPANY_NAME, file );
	SaveDynamicString ( boss, SIZE_PERSON_NAME, file );
	SaveDynamicString ( administrator, SIZE_PERSON_NAME, file );

	fwrite ( &size, sizeof(size), 1, file );
	fwrite ( &TYPE, sizeof(TYPE), 1, file );
	fwrite ( &growth, sizeof(growth), 1, file );
	fwrite ( &alignment, sizeof(alignment), 1, file );

	for ( int i = 0; i < 12; ++i ) 
		fwrite ( &sharehistory [i], sizeof(sharehistory [i]), 1, file );

	fwrite ( &lastmonthset, sizeof(lastmonthset), 1, file );

	SaveID_END ( file );

}

void Company::Print ()
{

	printf ( "Company : %s\n", name );	
	printf ( "\tSize=%d, type=%d, Growth=%d, Alignment=%d\n", size, TYPE, growth, alignment );
	printf ( "Boss    : %s\n", boss );
	printf ( "Admin   : %s\n", administrator );

	printf ( "Share history\n" );
	for ( int i = 0; i < 12; ++i ) 
		printf ( "%d:%dc  ", i, sharehistory [i] );

	printf ( "Last month set : %d\n", lastmonthset );

}

void Company::Update ()
{
}
	
char *Company::GetID ()
{

	return "COMPANY";

}

int Company::GetOBJECTID ()
{

	return OID_COMPANY;

}


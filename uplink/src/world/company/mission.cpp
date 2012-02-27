
#include <string.h>

#include "gucci.h"

#include "game/game.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "world/world.h"
#include "world/company/mission.h"
#include "world/vlocation.h"
#include "world/generator/missiongenerator.h"

#include "mmgr.h"


Mission::Mission ()
{

	details = NULL;
	fulldetails = NULL;
	duedate = NULL;
    
	whysomuchmoney = NULL;
	howsecure = NULL;
	whoisthetarget = NULL;
	
	TYPE = MISSION_NONE;
	UplinkStrncpy ( description, "", sizeof ( description ) );
	UplinkStrncpy ( employer, "", sizeof ( employer ) );
	UplinkStrncpy ( contact, "", sizeof ( contact ) );

	UplinkStrncpy ( completionA, "", sizeof ( completionA ) );
	UplinkStrncpy ( completionB, "", sizeof ( completionB ) );
	UplinkStrncpy ( completionC, "", sizeof ( completionC ) );
	UplinkStrncpy ( completionD, "", sizeof ( completionD ) );
	UplinkStrncpy ( completionE, "", sizeof ( completionE ) );

	payment = 0;
	maxpayment = 0;
	paymentmethod = MISSIONPAYMENT_AFTERCOMPLETION;

    difficulty = 0;
    minuplinkrating = -1;
    acceptrating = 0;
	npcpriority = false;

}

Mission::~Mission ()
{

	if ( details )			delete [] details;
	if ( fulldetails )		delete [] fulldetails;
	if ( whysomuchmoney )	delete [] whysomuchmoney;
	if ( howsecure )		delete [] howsecure;
	if ( whoisthetarget )	delete [] whoisthetarget;

	if ( duedate ) delete duedate;

	DeleteLListData ( &links );
	DeleteBTreeData ( &codes );

}

void Mission::SetTYPE ( int newTYPE )
{

	TYPE = newTYPE;

}

void Mission::SetCompletion ( char *newA, char *newB, char *newC, char *newD, char *newE )
{

	// No need to set them if NULL is passed in - they will not be examined after all

	if ( newA ) {
		UplinkAssert ( strlen(newA) < SIZE_MISSION_COMPLETION );
		UplinkStrncpy ( completionA, newA, sizeof ( completionA ) );
	}

	if ( newB ) {
		UplinkAssert ( strlen(newB) < SIZE_MISSION_COMPLETION );
		UplinkStrncpy ( completionB, newB, sizeof ( completionB ) );
	}

	if ( newC ) {
		UplinkAssert ( strlen(newC) < SIZE_MISSION_COMPLETION );
		UplinkStrncpy ( completionC, newC, sizeof ( completionC ) );
	}

	if ( newD ) {
		UplinkAssert ( strlen(newD) < SIZE_MISSION_COMPLETION );
		UplinkStrncpy ( completionD, newD, sizeof ( completionD ) );
	}

	if ( newE ) {
		UplinkAssert ( strlen(newE) < SIZE_MISSION_COMPLETION );
		UplinkStrncpy ( completionE, newE, sizeof ( completionE ) );
	}

}

void Mission::SetCreateDate ( Date *date )
{

	UplinkAssert ( date );
	createdate.SetDate ( date );

}

void Mission::SetNpcPriority ( bool newpriority )
{

	npcpriority = newpriority;

}

void Mission::SetDescription ( char *newdescription )
{

	UplinkAssert ( strlen ( newdescription ) < SIZE_MISSION_DESCRIPTION );
	UplinkStrncpy ( description, newdescription, sizeof ( description ) );

}

void Mission::SetDetails ( char *newdetails )
{

	if ( details ) delete [] details;

	details = new char [strlen(newdetails)+1];
	UplinkSafeStrcpy ( details, newdetails );

}

void Mission::SetFullDetails ( char *newdetails )
{

	if ( fulldetails ) delete [] fulldetails;

	fulldetails = new char [strlen(newdetails)+1];
	UplinkSafeStrcpy ( fulldetails, newdetails );

}

void Mission::SetWhySoMuchMoney	( char *answer )
{

	if ( whysomuchmoney ) delete [] whysomuchmoney;
	whysomuchmoney = NULL;

	if ( answer ) {

		whysomuchmoney = new char [strlen(answer)+1];
		UplinkSafeStrcpy ( whysomuchmoney, answer );

	}

}

void Mission::SetHowSecure ( char *answer )
{

	if ( howsecure ) delete [] howsecure;
	howsecure = NULL;

	if ( answer ) {

		howsecure = new char [strlen(answer)+1];
		UplinkSafeStrcpy ( howsecure, answer );

	}

}

void Mission::SetWhoIsTheTarget ( char *answer )
{

	if ( whoisthetarget ) delete [] whoisthetarget;
	whoisthetarget = NULL;

	if ( answer ) {

		whoisthetarget = new char [strlen(answer)+1];
		UplinkSafeStrcpy ( whoisthetarget, answer );

	}

}

void Mission::SetEmployer ( char *newemployer )
{

	UplinkAssert ( strlen (newemployer) < 64 );
	UplinkAssert ( game->GetWorld ()->GetCompany (newemployer) );

	UplinkStrncpy ( employer, newemployer, sizeof ( employer ) );

}

void Mission::SetContact ( char *newcontact )
{

	UplinkAssert ( strlen (newcontact) < SIZE_PERSON_NAME );
	UplinkAssert ( game->GetWorld ()->GetPerson ( newcontact ) );

	UplinkStrncpy ( contact, newcontact, sizeof ( contact ) );

}

void Mission::SetPayment ( int newpayment, int newmaxpayment )
{

	payment = newpayment;

	if ( newmaxpayment != -1 )
		maxpayment = newmaxpayment;

	else
		maxpayment = payment;

}

void Mission::SetDifficulty ( int newdifficulty )
{

	difficulty = newdifficulty;

}

void Mission::SetMinRating ( int newrating )
{

	minuplinkrating = newrating;

}

void Mission::SetAcceptRating ( int newrating )
{

	acceptrating = newrating;

}

void Mission::GiveLink ( char *ip )
{

	UplinkAssert ( strlen (ip) < SIZE_VLOCATION_IP );
	size_t theipsize = SIZE_VLOCATION_IP;
	char *theip = new char [theipsize];
	UplinkStrncpy ( theip, ip, theipsize );
	links.PutData ( theip );

}

void Mission::GiveCode ( char *ip, char *code )
{

	UplinkAssert ( strlen (ip) < SIZE_VLOCATION_IP );
	char theip [SIZE_VLOCATION_IP];
	UplinkStrncpy ( theip, ip, sizeof ( theip ) );

	char *thecode = new char [strlen(code)+1];
	UplinkSafeStrcpy ( thecode, code );

	codes.PutData ( theip, thecode );

}

void Mission::SetDueDate ( Date *newdate )
{

	if ( !duedate )
		duedate = new Date();

	duedate->SetDate( newdate );

}

Date *Mission::GetDueDate ()
{

	return duedate;

}

char *Mission::GetDetails ()
{

	UplinkAssert ( details );
	return details;

}

char *Mission::GetFullDetails ()
{

	UplinkAssert ( fulldetails );
	return fulldetails;

}

char *Mission::GetWhySoMuchMoney ()
{

	return whysomuchmoney;

}

char *Mission::GetHowSecure	()
{

	return howsecure;

}

char *Mission::GetWhoIsTheTarget ()
{

	return whoisthetarget;

}

bool Mission::Load  ( FILE *file )
{

	LoadID ( file );

	duedate = NULL;

	if ( !FileReadData ( &TYPE, sizeof(TYPE), 1, file ) ) return false;
	
	if ( !LoadDynamicStringStatic ( completionA, SIZE_MISSION_COMPLETION, file ) ) return false;
	if ( !LoadDynamicStringStatic ( completionB, SIZE_MISSION_COMPLETION, file ) ) return false;
	if ( !LoadDynamicStringStatic ( completionC, SIZE_MISSION_COMPLETION, file ) ) return false;
	if ( !LoadDynamicStringStatic ( completionD, SIZE_MISSION_COMPLETION, file ) ) return false;
	if ( !LoadDynamicStringStatic ( completionE, SIZE_MISSION_COMPLETION, file ) ) return false;

	if ( !createdate.Load ( file ) ) return false;

	if ( !LoadDynamicStringStatic ( employer, SIZE_MISSION_EMPLOYER, file ) ) return false;
	if ( !LoadDynamicStringStatic ( contact, SIZE_PERSON_NAME, file ) ) return false;
	if ( !LoadDynamicStringStatic ( description, SIZE_MISSION_DESCRIPTION, file ) ) return false;

	if ( !FileReadData ( &payment, sizeof(payment), 1, file ) ) return false;
	if ( !FileReadData ( &difficulty, sizeof(difficulty), 1, file ) ) return false;
	if ( !FileReadData ( &minuplinkrating, sizeof(minuplinkrating), 1, file ) ) return false;
	if ( !FileReadData ( &acceptrating, sizeof(acceptrating), 1, file ) ) return false;
	if ( !FileReadData ( &npcpriority, sizeof(npcpriority), 1, file) ) return false;
	
	if ( !LoadDynamicStringPtr ( &details, file ) ) return false;
	if ( !LoadDynamicStringPtr ( &fulldetails, file ) ) return false;
	if ( !LoadDynamicStringPtr ( &whysomuchmoney, file ) ) return false;
	if ( !LoadDynamicStringPtr ( &howsecure, file ) ) return false;
	if ( !LoadDynamicStringPtr ( &whoisthetarget, file ) ) return false;
	
	if ( !LoadLList ( &links, file ) ) return false;
	if ( !LoadBTree ( &codes, file ) ) return false;

	bool hasduedate;
	if ( !FileReadData ( &hasduedate, sizeof(hasduedate), 1, file ) ) return false;

	if ( hasduedate ) {
		duedate = new Date ();
		if ( !duedate->Load ( file ) ) return false;
	}

	if ( strcmp( game->GetLoadedSavefileVer(), "SAV57" ) >= 0 ) {

		if ( !FileReadData ( &maxpayment, sizeof(maxpayment), 1, file ) ) return false;
		if ( !FileReadData ( &paymentmethod, sizeof(paymentmethod), 1, file ) ) return false;

	}

	LoadID_END ( file );

	return true;

}

void Mission::Save  ( FILE *file )
{

	SaveID ( file );

	fwrite ( &TYPE, sizeof(TYPE), 1, file );
	
	SaveDynamicString ( completionA, SIZE_MISSION_COMPLETION, file );
	SaveDynamicString ( completionB, SIZE_MISSION_COMPLETION, file );
	SaveDynamicString ( completionC, SIZE_MISSION_COMPLETION, file );
	SaveDynamicString ( completionD, SIZE_MISSION_COMPLETION, file );
	SaveDynamicString ( completionE, SIZE_MISSION_COMPLETION, file );

	createdate.Save ( file );

	SaveDynamicString ( employer, SIZE_MISSION_EMPLOYER, file );
	SaveDynamicString ( contact, SIZE_PERSON_NAME, file );
	SaveDynamicString ( description, SIZE_MISSION_DESCRIPTION, file );

	fwrite ( &payment, sizeof(payment), 1, file );
	fwrite ( &difficulty, sizeof(difficulty), 1, file );
	fwrite ( &minuplinkrating, sizeof(minuplinkrating), 1, file );
	fwrite ( &acceptrating, sizeof(acceptrating), 1, file );
	fwrite ( &npcpriority, sizeof(npcpriority), 1, file );
	
	SaveDynamicString ( details, file );
	SaveDynamicString ( fulldetails, file );
	SaveDynamicString ( whysomuchmoney, file );
	SaveDynamicString ( howsecure, file );
	SaveDynamicString ( whoisthetarget, file );

	SaveLList ( &links, file );
	SaveBTree ( &codes, file );

	bool hasduedate = (duedate != NULL);
	fwrite ( &hasduedate, sizeof(hasduedate), 1, file );
	if ( hasduedate ) 	duedate->Save ( file );

	fwrite ( &maxpayment, sizeof(maxpayment), 1, file );
	fwrite ( &paymentmethod, sizeof(paymentmethod), 1, file );
	
	SaveID_END ( file );

}

void Mission::Print ()
{

	printf ( "Mission : TYPE=%d\n", TYPE );
	printf ( "Employer=%s, Payment=%d, Difficulty=%d, MinRating=%d, AcceptRating=%d, Description=%s\n",
			 employer, payment, difficulty, minuplinkrating, acceptrating, description );
	printf ( "NPC priority = %d\n", npcpriority );
	createdate.Print ();
	printf ( "\tCompletionA = %s\n", completionA );
	printf ( "\tCompletionB = %s\n", completionB );
	printf ( "\tCompletionC = %s\n", completionC );
	printf ( "\tCompletionD = %s\n", completionD );
	printf ( "\tCompletionE = %s\n", completionE );
	printf ( "\tContact = %s\n", contact );
	printf ( "\tDetails=%s\n", details );
	printf ( "\tFullDetails=%s\n", fulldetails );
	printf ( "\tWhySoMuchMoney=%s\n", whysomuchmoney );
	printf ( "\tHowSecure=%s\n", howsecure );

	PrintLList ( &links );
	PrintBTree ( &codes );

	if ( duedate ) {
		printf ( "Due date :\n" );
		duedate->Print ();
	}
	else 
		printf ( "No due date\n" );

}
	
char *Mission::GetID ()
{

	return "MISSION";

}

int Mission::GetOBJECTID ()
{

	return OID_MISSION;

}


// Message.cpp: implementation of the Message class.
//
//////////////////////////////////////////////////////////////////////

#include "gucci.h"

#include "game/game.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "world/world.h"
#include "world/message.h"
#include "world/computer/databank.h"

#include "mmgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Message::Message()
{
	
	subject = NULL;
	body = NULL;

	SetTo ( " " );
	SetFrom ( " " );
	SetSubject ( " " );
	SetBody ( " " );

	data = NULL;

}
 
Message::~Message()
{

	if ( subject ) delete [] subject;
	if ( body ) delete [] body;
	if ( data ) delete data;

    DeleteLListData ( &links );
    DeleteBTreeData ( &codes );

}

void Message::SetTo	( char *newto )
{

	UplinkAssert ( strlen (newto) < SIZE_PERSON_NAME );
	UplinkStrncpy ( to, newto, sizeof ( to ) );

}

void Message::SetFrom ( char *newfrom )
{

	UplinkAssert ( strlen (newfrom) < SIZE_PERSON_NAME );
	UplinkStrncpy ( from, newfrom, sizeof ( from ) );

}

void Message::SetSubject ( char *newsubject )
{

	if ( subject ) delete [] subject;
	subject = new char [ strlen(newsubject) + 1 ];
	UplinkSafeStrcpy ( subject, newsubject );

}

void Message::SetBody ( char *newbody )
{

	if ( body ) delete [] body;
	body = new char [ strlen (newbody) + 1 ];
	UplinkSafeStrcpy ( body, newbody );

}

void Message::SetDate ( Date *newdate )
{

	date.SetDate ( newdate );

}

void Message::GiveLink ( char *ip )
{

	size_t theipsize = SIZE_VLOCATION_IP;
	char *theip = new char [theipsize];
	UplinkAssert (strlen(ip) < SIZE_VLOCATION_IP );
	UplinkStrncpy ( theip, ip, theipsize );
	links.PutData (theip);

}

void Message::GiveCode ( char *ip, char *code )
{

	char *thecode = new char [strlen(code)+1];
	UplinkSafeStrcpy ( thecode, code );
    codes.PutData ( ip, thecode );

}

void Message::AttachData ( Data *newdata )
{

	if ( data )
		delete data;
	// Copy newdata
	data = new Data ( newdata );	

}

Data *Message::GetData ()
{

	return data;

}

char *Message::GetSubject ()
{

	UplinkAssert ( subject );
	return subject;

}

char *Message::GetBody ()
{

	UplinkAssert ( body );
	return body;

}

void Message::Send ()
{

	SetDate ( &(game->GetWorld ()->date) );

	Person *person = game->GetWorld ()->GetPerson ( to );

	if ( person ) {

		person->GiveMessage ( this );

	}
	else {

		// Send an error message to the sender
		Message *msg = new Message ();
		msg->SetTo ( from );
		msg->SetFrom ( "MAIL SERVER" );
		msg->SetSubject ( "Failed to deliver this email - unknown recipient" );
		msg->SetBody ( body );
		msg->Send ();

	}

}

bool Message::Load ( FILE *file )
{

	LoadID ( file );

	if ( !LoadDynamicStringStatic ( to, SIZE_PERSON_NAME, file ) ) return false;
	if ( !LoadDynamicStringStatic ( from, SIZE_PERSON_NAME, file ) ) return false;

	if ( subject ) {
		delete [] subject;
		subject = NULL;
	}
	if ( !LoadDynamicStringPtr ( &subject, file ) ) return false;
	if ( body ) {
		delete [] body;
		body = NULL;
	}
	if ( !LoadDynamicStringPtr ( &body, file ) ) return false;

	if ( !date.Load ( file ) ) return false;

	if ( !LoadLList ( &links, file ) ) return false;
	if ( !LoadBTree ( &codes, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void Message::Save ( FILE *file )
{

	SaveID ( file );

	SaveDynamicString ( to, SIZE_PERSON_NAME, file );
	SaveDynamicString ( from, SIZE_PERSON_NAME, file );

	SaveDynamicString ( subject, file );
	SaveDynamicString ( body, file );

	date.Save ( file );

	SaveLList ( &links, file );
	SaveBTree ( &codes, file );

	SaveID_END ( file );

}

void Message::Print ()
{

	printf ( "Message : \n" );
	printf ( "\tTo   = %s\n", to );
	printf ( "\tFrom = %s\n", from );
	printf ( "\tSubject = %s\n", subject );
	printf ( "\tbody = %s\n", body );

	PrintLList ( &links );
	PrintBTree ( &codes );

	date.Print ();

}
	
char *Message::GetID ()
{

	return "MESSAGE";

}

int Message::GetOBJECTID ()
{

	return OID_MESSAGE;

}

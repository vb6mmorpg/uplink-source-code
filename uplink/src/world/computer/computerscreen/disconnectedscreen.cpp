
#include "app/globals.h"
#include "app/serialise.h"

#include "world/vlocation.h"
#include "world/computer/computerscreen/disconnectedscreen.h"

#include "mmgr.h"

DArray <char *> DisconnectedScreen::loginslost;


DisconnectedScreen::DisconnectedScreen()
{
	
	nextpage = -1;
	textmessage = NULL;

}

DisconnectedScreen::~DisconnectedScreen()
{
	
	if ( textmessage ) delete [] textmessage;

}

void DisconnectedScreen::SetNextPage ( int newnextpage )
{

	nextpage = newnextpage;

}

void DisconnectedScreen::SetTextMessage ( char *newtextmessage )
{

	if ( textmessage ) delete [] textmessage;
	textmessage = new char [strlen(newtextmessage)+1];
	UplinkSafeStrcpy ( textmessage, newtextmessage );

}

void DisconnectedScreen::AddLoginLost ( char *ip )
{

	size_t ipcopysize = SIZE_VLOCATION_IP;
    char *ipcopy = new char [ipcopysize];
    UplinkStrncpy ( ipcopy, ip, ipcopysize );
    loginslost.PutData ( ipcopy );

}

void DisconnectedScreen::ClearLoginsLost ()
{

    DeleteDArrayData ( &loginslost );
    loginslost.Empty ();

}

bool DisconnectedScreen::Load  ( FILE *file )
{

	LoadID ( file );

	if ( !ComputerScreen::Load ( file ) ) return false;

	if ( !FileReadData ( &nextpage, sizeof(nextpage), 1, file ) ) return false;	
	if ( !LoadDynamicStringPtr ( &textmessage, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void DisconnectedScreen::Save  ( FILE *file )
{

	SaveID ( file );

	ComputerScreen::Save ( file );

	fwrite ( &nextpage, sizeof(nextpage), 1, file );
	SaveDynamicString ( textmessage, file );

	SaveID_END ( file );

}

void DisconnectedScreen::Print ()
{

	printf ( "DisconnectedScreen : \n" );
	ComputerScreen::Print ();
	printf ( "NextPage = %d, TextMessage = %s\n", 
				nextpage, textmessage );

}
	
char *DisconnectedScreen::GetID ()
{

	return "SCR_DISC";

}

int DisconnectedScreen::GetOBJECTID ()
{

	return OID_DISCONNECTEDSCREEN;

}
			


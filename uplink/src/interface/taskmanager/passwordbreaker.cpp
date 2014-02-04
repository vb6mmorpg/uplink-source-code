
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h> /*_glu_extention_library_*/

#include <string.h>
#include <stdio.h>

#include "soundgarden.h"
#include "gucci.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"
#include "app/opengl_interface.h"

#include "game/game.h"
#include "game/data/data.h"

#include "interface/interface.h"
#include "interface/taskmanager/taskmanager.h"
#include "interface/taskmanager/passwordbreaker.h"

#include "world/world.h"
#include "world/date.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/recordbank.h"
#include "world/computer/computerscreen/passwordscreen.h"
#include "world/computer/computerscreen/useridscreen.h"
#include "world/generator/numbergenerator.h"

#include "mmgr.h"


static char alphabet [27] = "abcdefghijklmnopqrstuvwxyz";


PasswordBreaker::PasswordBreaker () : UplinkTask ()
{

	length = difficulty = currentchar = 0;
	caption = password = username = NULL;
	found = NULL;

	targetstring = NULL;

}

PasswordBreaker::~PasswordBreaker()
{

	if ( caption )  delete [] caption;
	if ( username ) delete [] username;
	if ( password ) delete [] password;
	if ( found )    delete [] found;

	if ( targetstring ) delete [] targetstring;

}

void PasswordBreaker::Initialise ()
{

	if ( caption )
		delete [] caption;
	size_t captionsize = 32;
	caption = new char [captionsize];
	UplinkStrncpy ( caption, "Select target", captionsize );

}

void PasswordBreaker::SetTarget ( UplinkObject *uo, char *uos, int uoi )
{

	/*

		uo  : Target PasswordScreen or UserIDScreen object
		uos : name of button with password in
		uoi : unused

		*/

	targetobject = uo;
	if ( targetstring ) {
		delete [] targetstring;
		targetstring = NULL;
	}
	if ( uos ) {
		targetstring = new char[ strlen( uos ) + 1 ];
		UplinkSafeStrcpy( targetstring, uos );
	}
	targetint    = uoi;

    Computer *comp = NULL;

	if ( uo->GetOBJECTID () == OID_PASSWORDSCREEN ) {

		PasswordScreen *ps = (PasswordScreen *) uo;
	    comp = ps->GetComputer ();
	    UplinkAssert (comp);

		length = (int) strlen ( ps->password );
		difficulty = ps->difficulty;
		
		if ( password )
			delete [] password;
		size_t passwordsize = length+1;
		password = new char [passwordsize];
		UplinkStrncpy ( password, ps->password, passwordsize );

		if ( username )
			delete [] username;
		username = NULL;

	}
	else if ( uo->GetOBJECTID () == OID_USERIDSCREEN ) {

	    UserIDScreen *uid = (UserIDScreen *) uo;
	    comp = uid->GetComputer ();
	    UplinkAssert (comp);

		// If the player has not entered a UserID then do this for admin
		UplinkAssert ( EclGetButton ( "useridscreen_name" ) );
		if ( strcmp ( EclGetButton ( "useridscreen_name" )->caption, "" ) == 0 ) 
			EclGetButton ( "useridscreen_name" )->SetCaption ( "admin" );
				
		if ( username )
			delete [] username;
		char *name = EclGetButton ( "useridscreen_name" )->caption;
		username = new char [strlen(name)+1];
		UplinkSafeStrcpy ( username, name );

		// Look up the user name in this computer's records
				
		Record *rec = comp->recordbank.GetRecordFromName ( name );
		char *recPassword = NULL;
		char *recSecurity = NULL;

		// It's possible that RECORDBANK_PASSWORD or RECORDBANK_SECURITY don't exists,
		// the accounts in the Global Criminal Database don't have them.

		if ( rec ) {
			recPassword = rec->GetField ( RECORDBANK_PASSWORD );
			recSecurity = rec->GetField ( RECORDBANK_SECURITY );
		}

		if ( rec && recPassword && recSecurity ) {

			char *passwd = recPassword;
			int security;
			sscanf ( recSecurity, "%d", &security );

			difficulty = (int) ( HACKDIFFICULTY_SECURITYMODIFIER [security] * uid->difficulty );

			length = (int) strlen ( passwd );
			
			if ( password )
				delete [] password;
			size_t passwordsize = length+1;
			password = new char [passwordsize];
			UplinkStrncpy ( password, passwd, passwordsize );
						
		}
		else {

			// The user name is not recognised
			SvbRemoveTask ( this );
			return;

		}

	}
	else {

		// Unrecognised object
		return;

	}

	// Set up the password breaker to work with this new data

	if ( caption )
		delete [] caption;
	size_t captionsize = length+1;
	caption = new char [captionsize];
	if ( found )
		delete [] found;
	found = new bool [length];

	UplinkStrncpy ( caption, "", captionsize );
	for ( int i = 0; i < length; ++i ) {
		UplinkStrncat ( caption, captionsize, "x" );
		found [i] = false;
	}

    int pid = SvbLookupPID (this);

	char buttonname [64];
	UplinkSnprintf ( buttonname, sizeof ( buttonname ), "passwordbreaker %d", pid );
	EclGetButton ( buttonname )->width = length * 26;

	char closename [64];
	UplinkSnprintf ( closename, sizeof ( closename ), "passwordbreaker_close %d", pid );
	EclGetButton ( closename )->x = EclGetButton(buttonname)->x + (length * 26) - 13;
    EclDirtyButton ( closename );

    char fillname [64];
    UplinkSnprintf ( fillname, sizeof ( fillname ), "passwordbreaker_fill %d", pid );
    EclGetButton ( fillname )->width = (length * 26) - 14;
    EclDirtyButton ( fillname );

    if ( difficulty == -1 )     numticksrequired = -1;
    else                	    numticksrequired = NumberGenerator::ApplyVariance ( difficulty, (int) ( HACKDIFFICULTY_VARIANCE * 100 ) );

	progress = 0;

    UplinkAssert (comp);

	if ( comp->security.IsRunning_Monitor () )
		game->GetWorld ()->GetPlayer ()->GetConnection ()->BeginTrace ();

}

void PasswordBreaker::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		if ( length > 0 ) {

			for ( int count = 0; count < n; ++count ) {


				// First check to make sure we are still looking at this password screen
				// i.e the player has not logged out / moved / got past it already

				if ( !EclGetButton ( targetstring ) ) {

					// Target password box has been removed; Terminate this task
					SvbRemoveTask ( this );			

					// WARNING - This instance has been deleted - do not access any more data
					return;

				}
				
				if ( currentchar < length ) {

					// Some of the word has not been calculated
					// Do it now

					// First see if we have calculated the current character

					++progress;

					if ( numticksrequired != -1 && progress >= numticksrequired ) {

						// This char has been discovered
						caption [currentchar] = password [currentchar];
						found [currentchar] = true;
						currentchar++;

						SgPlaySound ( RsArchiveFileOpen ( "sounds/done.wav" ), "sounds/done.wav" );

						// Work out when the next one will be discovered
						//int timetonextletter = NumberGenerator::ApplyVariance ( difficulty, HACKDIFFICULTY_VARIANCE * 100 );
						//nextletter.SetDate ( &(game->GetWorld ()->date) );
						//nextletter.AdvanceSecond ( timetonextletter );

						numticksrequired = NumberGenerator::ApplyVariance ( difficulty, (int) ( HACKDIFFICULTY_VARIANCE * 100 ) );
						progress = 0;

					}
					

					// Now refresh the caption with random chars where appropriate
					for ( int i = currentchar; i < length; ++i )
						caption [i] = alphabet [ int ( 26 * ( (float) rand () / (float) RAND_MAX ) ) ];
					
					if ( IsInterfaceVisible () ) {
						char buttonname [64];
						UplinkSnprintf ( buttonname, sizeof ( buttonname ), "passwordbreaker %d", SvbLookupPID (this) );
						EclGetButton ( buttonname )->SetCaption ( caption );
						GciPostRedisplay ();
					}


				}
				else {

					// The full word has been discovered - enter it into the password screen

					EclRegisterCaptionChange ( targetstring, password );

					// Give the code to the player

					char newcode [128];
                    if ( username ) Computer::GenerateAccessCode( username, password, newcode, sizeof ( newcode ) );
                    else			Computer::GenerateAccessCode( password, newcode, sizeof ( newcode ) );
					game->GetWorld ()->GetPlayer ()->GiveCode ( game->GetWorld ()->GetPlayer ()->remotehost, newcode );				

					// Remove this task

					SvbRemoveTask ( this );
				
					// WARNING - This instance has been deleted - do not access any more data
					return;

				}

			}

		}

	}

}

void PasswordBreaker::PasswordDraw ( Button *button, bool highlighted, bool clicked )
{

    /*

	glBegin ( GL_QUADS );

		if      ( clicked )		glColor4f ( 0.5f, 0.5f, 1.0f, ALPHA );
		else if ( highlighted ) glColor4f ( 0.2f, 0.2f, 0.9f, ALPHA );
		else					glColor4f ( 0.0f, 0.0f, 0.5f, ALPHA );
		glVertex2i ( button->x, button->y );

		if		( clicked )		glColor4f ( 0.7f, 0.7f, 1.0f, ALPHA );
		else if ( highlighted ) glColor4f ( 0.5f, 0.5f, 1.0f, ALPHA );
		else					glColor4f ( 0.0f, 0.0f, 0.7f, ALPHA );
		glVertex2i ( button->x + button->width, button->y );

		if		( clicked )		glColor4f ( 0.5f, 0.5f, 1.0f, ALPHA );
		else if ( highlighted ) glColor4f ( 0.2f, 0.2f, 0.9f, ALPHA );
		else					glColor4f ( 0.0f, 0.0f, 0.5f, ALPHA );
		glVertex2i ( button->x + button->width, button->y + button->height );

		if		( clicked )		glColor4f ( 0.7f, 0.7f, 1.0f, ALPHA );
		else if ( highlighted ) glColor4f ( 0.5f, 0.5f, 1.0f, ALPHA );
		else					glColor4f ( 0.0f, 0.0f, 0.7f, ALPHA );
		glVertex2i ( button->x, button->y + button->height );

	glEnd ();

    */

    BackgroundDraw ( button, highlighted, clicked );

	int xpos = button->x + 10;
	int ypos = button->y + 18;
		
	glColor4f ( 1.0f, 1.0f, 1.0f, 0.8f );    
    
	if ( strcmp ( button->caption, "Select target" ) != 0 ) {

		for ( size_t i = 0; i < strlen (button->caption); ++i ) {
			char caption [2];
			UplinkSnprintf ( caption, sizeof ( caption ), "%c", button->caption [i] );
			GciDrawText ( (int) ( xpos + i * 25 ), ypos, caption, BITMAP_15 );
		}

	}
	else {
		
		GciDrawText ( xpos, ypos, button->caption, HELVETICA_12 );

	}

}

void PasswordBreaker::PasswordClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	game->GetInterface ()->GetTaskManager ()->SetTargetProgram ( pid );

}

void PasswordBreaker::CloseClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	SvbRemoveTask ( pid );

}

void PasswordBreaker::BackgroundDraw ( Button *button, bool highlighted, bool clicked )
{

	glBegin ( GL_QUADS );		
		glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x, button->y + button->height );
		glColor3ub ( 8, 20, 124 );		glVertex2i ( button->x, button->y );
		glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x + button->width, button->y );
		glColor3ub ( 8, 20, 124 );		glVertex2i ( button->x + button->width, button->y + button->height );
	glEnd ();

	glColor3ub ( 81, 138, 215 );
	border_draw ( button );
	
}

void PasswordBreaker::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID (this);

        char buttonname [64];
		UplinkSnprintf ( buttonname, sizeof ( buttonname ), "passwordbreaker %d", pid );
		EclRegisterButton ( 245, 375, 130, 30, caption, "Password Breaker", buttonname );
		EclRegisterButtonCallbacks ( buttonname, PasswordDraw, PasswordClick, button_click, button_highlight );

        char fillname [64];
        UplinkSnprintf ( fillname, sizeof ( fillname ), "passwordbreaker_fill %d", pid );
        EclRegisterButton ( 245, 360, 116, 15, "Password Breaker", "", fillname );
        EclRegisterButtonCallbacks ( fillname, button_draw, NULL, NULL, NULL );

		char closename [64];
		UplinkSnprintf ( closename, sizeof ( closename ), "passwordbreaker_close %d", pid );
    	EclRegisterButton ( 362, 361, 13, 13, "X", "Close the password breaker", closename );
		button_assignbitmaps ( closename, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( closename, CloseClick );

	}

}

void PasswordBreaker::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char buttonname [64];
		UplinkSnprintf ( buttonname, sizeof ( buttonname ), "passwordbreaker %d", pid );
		EclRemoveButton ( buttonname );

		char closename [64];
		UplinkSnprintf ( closename, sizeof ( closename ), "passwordbreaker_close %d", pid );
		EclRemoveButton ( closename );

        char fillname [64];
        UplinkSnprintf ( fillname, sizeof ( fillname ), "passwordbreaker_fill %d", pid );
        EclRemoveButton ( fillname );

	}

}

void PasswordBreaker::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID (this);

	char buttonname [64];
    UplinkSnprintf ( buttonname, sizeof ( buttonname ), "passwordbreaker %d", pid );
	EclButtonBringToFront ( buttonname );

	char closename [64];
	UplinkSnprintf ( closename, sizeof ( closename ), "passwordbreaker_close %d", pid );
	EclButtonBringToFront ( closename );

    char fillname [64];
    UplinkSnprintf ( fillname, sizeof ( fillname ), "passwordbreaker_fill %d", pid );
    EclButtonBringToFront ( fillname );

}

bool PasswordBreaker::IsInterfaceVisible ()
{
	
	char buttonname [64];
	UplinkSnprintf ( buttonname, sizeof ( buttonname ), "passwordbreaker %d", SvbLookupPID (this) );

	return ( EclGetButton ( buttonname ) != NULL );

}



#ifdef WIN32
#include <windows.h>
#endif

#ifndef HAVE_GLES
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GLES/gl.h>
#include <GLES/glues.h>
#endif


#include <stdlib.h>

#include "gucci.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "mainmenu/mainmenu.h"
#include "mainmenu/theteam_interface.h"

#include "world/generator/numbergenerator.h"

#include "mmgr.h"

// Valid characters for teamnames:
// a - z
// A - Z
// <space>
// <comma>
// <fullstop>
// <nextline>

// Ordering of check :   Z -> A -> z -> a -> Z
// (Symbols are set immediately - only text cycles)


static char *teamnames[] = { 
							 "Chris Delay", 
							 "Tom Arundel",
							 "Mark Morris",
							 "John Knottenbelt",	
							};

/*
static char *teamwords[] = {
							 "Lead Developer\n\n"
							 "Pretty much responsible for everything you see and do in this game.  "
                             "Some might say he was the reason why this game is so self consciously pretentious at times.  "
                             "Times like this in fact.\n\n"
							 "The creative vision behind Introversion Software.\n"
							 "Its a lonely world.",

							 "[Supposedly] CEO\n\n"
							 "I have no idea what this person does.",

							 "Managing Director\n\n"
							 "Handles all of the things I dont want to do, like writing business plans, "
                             "talking to shop owners and stuff.\n\n"
							 "Cursed by his own intelligence and awareness, "
                             "this guy is almost certainly doomed to a life of extreme unhappiness.",

							 "Linux Support\n\n"
							 "The cleverest man I have (or will) ever meet.\n"
							 "Mostly responsible for all things related to that "
                             "god-forsaken joke of an operating system.\n\n"
							 "A great friend and source of inspiration as well."
							};
*/

static char *teamwords[] = {
                                "The last of the bedroom programmers\n"
                                "The first of the children\n"
                                "\n\n\n\n"
                                "and when wizards stay up late,\n"
                                "new worlds are born",

                                "Carpe diemLIVE in that moment\n"
                                "But dont hesitate to forget it later\n\n\n"
                                "Impulsive doesnotdoyou justice\n\n"
                                "Throw it away\n"
                                "Corner of the room\n"
                                "Bottom of the shelf\n"
                                "Behind last weeks wonder\n\n\n"
                                "It was good while it lasted\n"
                                "Was it worth it",

                                "The bird at the front of the flock\n\n"
                                "The pitch black tunnel\n"
                                "with no light at the end\n\n\n"
                                "What do you say to these people\n"
                                "What do you say to yourself\n\n\n\n"
                                "Oh God when will it ever end",

                                "You are the source\n"
                                "It comes from within\n\n\n\n"
                                "Dont worry about us"

                            };


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TheTeamInterface::TheTeamInterface()
{

	targetindex = -1;
	timesync = 0;

}

TheTeamInterface::~TheTeamInterface()
{

}

void TheTeamInterface::NameDraw ( Button *button, bool highlighted, bool clicked )
{
    
    glScissor ( button->x, 480 - (button->y + button->height), button->width, button->height );    
    glEnable ( GL_SCISSOR_TEST );

	clear_draw ( button->x, button->y, button->width, button->height );

	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
	int ypos = (button->y + button->height / 2) + 5;

	for ( size_t i = 0; i < strlen(button->caption); ++i ) {
		char thischar [2];
		UplinkSnprintf ( thischar, sizeof ( thischar ), "%c", button->caption [i] );
		GciDrawText ( (int) ( button->x + i * 12 ), ypos, thischar, HELVETICA_18 );
	}
		
	glDisable ( GL_SCISSOR_TEST );

}

void TheTeamInterface::TextDraw ( Button *button, bool highlighted, bool clicked )
{

    glScissor ( button->x, 480 - (button->y + button->height), button->width, button->height );    
    glEnable ( GL_SCISSOR_TEST );

	clear_draw ( button->x, button->y, button->width, button->height );

	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );

	LList <char *> *wrappedText = wordwraptext ( button->caption, button->width );
	UplinkAssert (wrappedText);

	for ( int line = 0; line < wrappedText->Size (); ++line ) {

		int ypos = button->y + 5 + line * 12;
		char *thisLine = wrappedText->GetData (line);
		UplinkAssert (thisLine);

		for ( size_t i = 0; i < strlen(thisLine); ++i ) {
			char thischar [2];
			UplinkSnprintf ( thischar, sizeof ( thischar ), "%c", thisLine [i] );
			GciDrawText ( (int) ( button->x + i * 6 ), ypos, thischar, HELVETICA_10 );
		}

	}

	if ( wrappedText->ValidIndex (0) && wrappedText->GetData (0) )
		delete [] wrappedText->GetData (0);				// Only delete first entry - since there is only one string really
	delete wrappedText;

	glDisable ( GL_SCISSOR_TEST );

}

void TheTeamInterface::TeamMemberHighlight ( Button *button )
{

	int index;
	sscanf ( button->name, "theteam_member %d", &index );

	TheTeamInterface *thisint = (TheTeamInterface *) app->GetMainMenu ()->GetMenuScreen ();
	UplinkAssert (thisint);

	if ( thisint->targetindex != index ) {

		thisint->targetindex = index;
		EclRegisterCaptionChange ( "theteam_details", teamwords [index-1] );

	}
	
}

void TheTeamInterface::ExitClick ( Button *button ) 
{

	app->GetMainMenu ()->RunScreen ( MAINMENU_LOGIN );

}

void TheTeamInterface::Create ()
{	

	if ( !IsVisible () ) {

		// Create the four pictures

		EclRegisterButton ( 10, 10, 200, 100, " ", " ", "theteam_member 1" );
		button_assignbitmap ( "theteam_member 1", "mainmenu/team1.tif" );
		EclRegisterButtonCallbacks ( "theteam_member 1", imagebutton_draw, NULL, NULL, TeamMemberHighlight );

		EclRegisterButton ( 430, 10, 200, 100, " ", " ", "theteam_member 3" );
		button_assignbitmap ( "theteam_member 3", "mainmenu/team3.tif" );
		EclRegisterButtonCallbacks ( "theteam_member 3", imagebutton_draw, NULL, NULL, TeamMemberHighlight );

		EclRegisterButton ( 430, 370, 200, 100, " ", " ", "theteam_member 2" );
		button_assignbitmap ( "theteam_member 2", "mainmenu/team2.tif" );
		EclRegisterButtonCallbacks ( "theteam_member 2", imagebutton_draw, NULL, NULL, TeamMemberHighlight );

		EclRegisterButton ( 10, 370, 200, 100, " ", " ", "theteam_member 4" );
		button_assignbitmap ( "theteam_member 4", "mainmenu/team4.tif" );
		EclRegisterButtonCallbacks ( "theteam_member 4", imagebutton_draw, NULL, NULL, TeamMemberHighlight );

		// Create the central textbox

		EclRegisterButton ( 180, 140, 280, 20, " ", " ", "theteam_name" );
		EclRegisterButtonCallbacks ( "theteam_name", NameDraw, NULL, NULL, NULL );

		EclRegisterButton ( 180, 170, 280, 300, " ", " ", "theteam_details" );
		EclRegisterButtonCallbacks ( "theteam_details", textbutton_draw, NULL, NULL, NULL );

		// Exit button

		EclRegisterButton ( 600, 465, 39, 14, "Exit", "Return to the main menu", "theteam_exit" );
		EclRegisterButtonCallback ( "theteam_exit", ExitClick );

	}

}

void TheTeamInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "theteam_member 1" );
		EclRemoveButton ( "theteam_member 2" );
		EclRemoveButton ( "theteam_member 3" );
		EclRemoveButton ( "theteam_member 4" );

		EclRemoveButton ( "theteam_name" );
		EclRemoveButton ( "theteam_details" );

		EclRemoveButton ( "theteam_exit" );

	}

}

void TheTeamInterface::Update ()
{

	if ( targetindex != -1 ) {

		if ( EclGetAccurateTime () > timesync ) {

			MergeCaption ( "theteam_name", teamnames[targetindex-1] );

			timesync = (int) ( EclGetAccurateTime () + 100 );
				
		}

	}

}

void TheTeamInterface::MergeCaption ( char *buttonName, char *targetCaption )
{

	UplinkAssert ( EclGetButton(buttonName) );
	char *currentCaption = EclGetButton (buttonName)->caption;
	char *newCaption = NULL;

	//
	// First check - is the current name the right length?
	//

	if ( strlen(currentCaption)	< strlen(targetCaption) ) {

		size_t newCaptionSize = strlen(currentCaption) + 2;
		newCaption = new char [newCaptionSize];
		UplinkStrncpy ( newCaption, currentCaption, newCaptionSize );
		newCaption [newCaptionSize-2] = 'A' + NumberGenerator::RandomNumber (26);
		newCaption [newCaptionSize-1] = '\x0';

	}
	else if ( strlen(currentCaption) > strlen(targetCaption) ) {

		size_t newCaptionSize = strlen(currentCaption)+1;
		newCaption = new char [newCaptionSize];
		strncpy ( newCaption, currentCaption, newCaptionSize );
		newCaption [newCaptionSize-1] = '\x0';

	}
	else {

		newCaption = new char [strlen(currentCaption)+1];
		UplinkSafeStrcpy ( newCaption, currentCaption );

	}


	//
	// Go through each letter, cycling as neccesary
	//

	UplinkAssert (newCaption);

	size_t newCaptionSize = strlen(newCaption);
	size_t targetCaptionSize = strlen(targetCaption);
	int numchanges = 0;

	for ( size_t i = 0; i < newCaptionSize; ++i ) {

		if ( i < targetCaptionSize ) {

			// We are inside the target caption
			// Cycle downwards if we aren't right yet

			if ( newCaption [i] != targetCaption [i] ) {

				++numchanges;

				if ( targetCaption [i] == ' ' || targetCaption [i] == '.' || targetCaption [i] == ',' || targetCaption [i] == '\n' ) {

					newCaption [i] = targetCaption [i];

				}
				else {

					if ( newCaption [i] <= 'A' )
						newCaption [i] = 'z';

					else if ( newCaption [i] == 'a' )
						newCaption [i] = 'Z';

					else 
						newCaption [i] = newCaption [i] - 1;

				}

			}

		}
		else {

			// This is outside the target caption
			// So just cycle it anyway
			
			++numchanges;

			if ( newCaption [i] <= 'A' )
				newCaption [i] = 'z';

			else if ( newCaption [i] == 'a' )
				newCaption [i] = 'Z';

			else
				newCaption [i] = newCaption [i] - 1;

		}

	}


	// 
	// If we have now finished, numchanges would have been zero
	//

	if ( numchanges == 0 ) {

		//targetindex = -1;
		return;

	}


	//
	// Finally, set the button to the new caption
	//

	EclGetButton ( buttonName )->SetCaption (newCaption);
	delete [] newCaption;

}

bool TheTeamInterface::IsVisible ()
{

	return ( EclGetButton ( "theteam_member 1" ) != 0 );

}

int TheTeamInterface::ScreenID ()
{

	return MAINMENU_THETEAM;

}

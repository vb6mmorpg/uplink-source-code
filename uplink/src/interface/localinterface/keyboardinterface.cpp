// KeyboardInterface.cpp: implementation of the KeyboardInterface class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "eclipse.h"
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"

#include "interface/localinterface/keyboardinterface.h"

#include "mmgr.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

char *KeyboardInterface::targetbuttonname = NULL;

KeyboardInterface::KeyboardInterface()
{

	targetbuttonname = NULL;

}

KeyboardInterface::~KeyboardInterface()
{

	if ( targetbuttonname ) delete [] targetbuttonname;

}

void KeyboardInterface::RemoveKeyboardInterface ( Button *button )
{

	for ( int dy = 0; dy < 3; ++dy ) {
		for ( int dx = 0; dx < 9; ++dx ) {
			
			if ( dy != 2 || dx != 8 ) {

				char letter [2];
				UplinkSnprintf ( letter, sizeof ( letter ), "%c", (char) ('a' + ( dy * 9 + dx )) );
				char name [64];
				UplinkSnprintf ( name, sizeof ( name ), "keyboard %s", letter );

				EclRemoveButton ( name );

			}

		}
	}
	
	EclRemoveButton ( "keyboard back" );
	EclRemoveButton ( "keyboard clear" );
	EclRemoveButton ( "keyboard space" );
	EclRemoveButton ( "keyboard close" );

}

void KeyboardInterface::Keypress ( Button *button )
{

	char letter [32];
	sscanf ( button->name, "keyboard %s", letter );
	Button *tbutton = EclGetButton ( targetbuttonname );

	if ( button ) {

		char newcaption [128];
		size_t newcaptionsize = min ( strlen ( tbutton->caption ) + 1, sizeof ( newcaption ) );
		strncpy ( newcaption, tbutton->caption, newcaptionsize );
		newcaption [ newcaptionsize - 1 ] = '\0';

		if ( strcmp ( letter, "back" ) == 0 ) {

			if ( newcaptionsize > 1 )
				newcaption [ newcaptionsize - 2 ] = '\0';

		}
		else if ( strcmp ( letter, "clear" ) == 0 ) {

			UplinkStrncpy ( newcaption, "", sizeof ( newcaption ) );

		}
		else if ( strcmp ( letter, "space" ) == 0 ) {

			if ( ( newcaptionsize + 1 ) <= sizeof ( newcaption ) )
				UplinkSafeStrcat ( newcaption, " " );
			
		}
		else {

			if ( ( newcaptionsize + strlen ( letter ) ) <= sizeof ( newcaption ) )
				UplinkSafeStrcat ( newcaption, letter );

		}

		tbutton->SetCaption ( newcaption );

	}

}

void KeyboardInterface::SetTargetButton ( char *bname ) 
{

	if ( targetbuttonname ) delete [] targetbuttonname;
	targetbuttonname = new char [ strlen (bname) + 1 ];
	UplinkSafeStrcpy ( targetbuttonname, bname );

}

void KeyboardInterface::Create ( int x, int y )
{
	
	if ( !IsVisible () ) {

		UplinkAssert ( targetbuttonname );

		for ( int dy = 0; dy < 3; ++dy ) {
			for ( int dx = 0; dx < 9; ++dx ) {
				
				if ( dy != 2 || dx != 8 ) {

					char letter [2];
					UplinkSnprintf ( letter, sizeof ( letter ), "%c", (char) ('a' + ( dy * 9 + dx )) );
					char name [64];
					UplinkSnprintf ( name, sizeof ( name ), "keyboard %s", letter );

					EclRegisterButton ( x + dx * 20, y + dy * 20, 18, 18, letter, "Press this key", name );
					EclRegisterButtonCallback ( name, Keypress );

				}

			}
		}

		EclRegisterButton ( x, y + 60, 38, 18, "Back", "Delete the last character", "keyboard back" );
		EclRegisterButton ( x + 40, y + 60, 38, 18, "Clear", "Clear the text box", "keyboard clear" );
		EclRegisterButton ( x + 80, y + 60, 38, 18, "Space", "Add a space", "keyboard space" );
		EclRegisterButton ( x + 120, y + 60, 38, 18, "Close", "Close the keyboard", "keyboard close" );

		EclRegisterButtonCallback ( "keyboard back", Keypress );
		EclRegisterButtonCallback ( "keyboard clear", Keypress );
		EclRegisterButtonCallback ( "keyboard space", Keypress );	
		EclRegisterButtonCallback ( "keyboard close", RemoveKeyboardInterface );

	}

}

void KeyboardInterface::Remove ()
{

	RemoveKeyboardInterface ( NULL );

}

bool KeyboardInterface::IsVisible ()
{

	return ( EclGetButton ( "keyboard back" ) != NULL );

}


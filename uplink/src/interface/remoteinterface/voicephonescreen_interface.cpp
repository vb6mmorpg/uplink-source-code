
#include <stdio.h>

#include "redshirt.h"
#include "soundgarden.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "world/world.h"
#include "world/computer/computer.h"
#include "world/computer/computerscreen/genericscreen.h"
#include "world/generator/numbergenerator.h"

#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/voicephonescreen_interface.h"

#include "mmgr.h"



VoicePhoneScreenInterface::VoicePhoneScreenInterface ()
{

	numrings = 0;
	timesync = -1;
    voiceindex = 0;
    STATUS = VOICESTATUS_NONE;
	answer = true;

}

VoicePhoneScreenInterface::~VoicePhoneScreenInterface ()
{
}


bool VoicePhoneScreenInterface::ReturnKeyPressed ()
{

	return false;

}

void VoicePhoneScreenInterface::AddToText ( char *newtext )
{
		
	UplinkAssert (newtext);
	Button *button = EclGetButton ("voicescreen_text");
	UplinkAssert (button);
	size_t newcaptionsize = strlen(button->caption) + strlen(newtext) + 3;
	char *newcaption = new char [newcaptionsize];
	UplinkSnprintf ( newcaption, newcaptionsize, "%s%s", button->caption, newtext );
	button->SetCaption ( newcaption );
	delete [] newcaption;

}

void VoicePhoneScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert (newcs);
	cs = newcs;

	if ( !IsVisible () ) {

        // Look up who this voice screen belongs to

        Computer *comp = GetComputerScreen ()->GetComputer ();
        UplinkAssert (comp);
        Person *person = game->GetWorld ()->GetPerson ( comp->companyname );
        UplinkAssert (person);
        voiceindex = person->voiceindex;

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "voicescreen_maintitle" );
		EclRegisterButtonCallbacks ( "voicescreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "voicescreen_subtitle" );
		EclRegisterButtonCallbacks ( "voicescreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

		EclRegisterButton ( 40, 140, 300, 250, " ", " ", "voicescreen_text" );
		EclRegisterButtonCallbacks ( "voicescreen_text", textbutton_draw, NULL, NULL, NULL );

		// Set up the screen data

		if ( person->GetStatus () != PERSON_STATUS_NONE ) {

			answer = false;
			numrings = 6 + NumberGenerator::RandomNumber (3);

		}
		else {

			answer = true;
			numrings = 2 + NumberGenerator::RandomNumber (3);

		}

		numwords = 3 + NumberGenerator::RandomNumber (2);
		STATUS = VOICESTATUS_RINGING;
		timesync = (int) ( EclGetAccurateTime () + 1000 );

	}

}

void VoicePhoneScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "voicescreen_maintitle" );
		EclRemoveButton ( "voicescreen_subtitle" );
		EclRemoveButton ( "voicescreen_text" );

	}

}

void VoicePhoneScreenInterface::Update ()
{

	if ( IsVisible () ) {

		if ( timesync != -1 && EclGetAccurateTime () > timesync ) {

			switch ( STATUS ) {

				case VOICESTATUS_RINGING: 
				{
	
					AddToText ( "\nRinging..." );
					--numrings;
					timesync = (int) ( EclGetAccurateTime () + 2500 );
					SgPlaySound ( RsArchiveFileOpen ( "sounds/ringout.wav" ), "sounds/ringout.wav", true );
					
					if ( numrings <= 0 ) {
						if ( answer )
							STATUS = VOICESTATUS_CONNECTED;
						else
							STATUS = VOICESTATUS_HUNGUP;
					}
					
				}
				break;

				case VOICESTATUS_CONNECTED:
				{

					AddToText ( "\n\nConnected" );
					AddToText ( "\nVoice channel open\n" );
					timesync = (int) ( EclGetAccurateTime () + 1500 );
					STATUS = VOICESTATUS_TALKING;
					SgPlaySound ( RsArchiveFileOpen ( "sounds/phonepickup.wav" ), "sounds/phonepickup.wav", true );

				}
				break;

				case VOICESTATUS_TALKING:
				{
					
					--numwords;
					timesync = (int) ( EclGetAccurateTime () + 2500 );
					if ( numwords <= 0 ) STATUS = VOICESTATUS_HUNGUP;
                    char filename [256];
                    int samplenumber = numwords;
                    if ( samplenumber >= 3 ) samplenumber -= 3;
//#ifndef DEMOGAME
                    UplinkSnprintf ( filename, sizeof ( filename ), "sounds/analyser/hello%d_%d.wav", voiceindex, samplenumber ); 
					SgPlaySound ( RsArchiveFileOpen ( filename ), filename, true );
//#endif

				}
				break;

				case VOICESTATUS_HUNGUP:
				{

					if ( answer ) {
						AddToText ( "\n\nConnection lost" );
						AddToText ( "\n(Receiver hung up)" );
					}
					else
						AddToText ( "\n\nNo answer" );
					timesync = -1;
					SgPlaySound ( RsArchiveFileOpen ( "sounds/phonepickup.wav" ), "sounds/phonepickup.wav", true );

				}
				break;

			}
			
		}

	}

}

bool VoicePhoneScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "voicescreen_maintitle" ) != NULL );

}


int VoicePhoneScreenInterface::ScreenID ()
{

	return SCREEN_VOICEPHONE;

}


GenericScreen *VoicePhoneScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);
	return (GenericScreen *) cs;

}

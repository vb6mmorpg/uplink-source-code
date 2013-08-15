

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h>

#include "vanbakel.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/serialise.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/computerscreen/genericscreen.h"
#include "world/computer/databank.h"
#include "world/generator/numbergenerator.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/voicephonescreen_interface.h"
#include "interface/remoteinterface/voiceanalysisscreen_interface.h"
#include "interface/taskmanager/taskmanager.h"
#include "interface/taskmanager/voiceanalyser.h"

#include "mmgr.h"

#define max(a,b) (((a) > (b)) ? (a) : (b)) 


VoiceAnalyser::VoiceAnalyser ()
{

	for ( int i = 0; i < VOICEANALYSER_NUMSAMPLES; ++i )
		sample [i] = 0;

	STATUS = VOICEANALYSER_STATUS_NONE;
	UplinkStrncpy ( personname, " ", sizeof ( personname ) );
	UplinkStrncpy ( personip, " ", sizeof ( personip ) );
	timesync = 0;
	animsync = 0;

	source = NULL;
	sourceindex = -1;
	numticksrequired = 0;
	progress = 0;
	remotefile = false;

}

VoiceAnalyser::~VoiceAnalyser ()
{
}

void VoiceAnalyser::CloseClick ( Button *button )
{

	UplinkAssert (button);

	char name [64];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	SvbRemoveTask ( pid );

}

void VoiceAnalyser::TitleClick ( Button *button )
{

	UplinkAssert (button);

	char name [64];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	VoiceAnalyser *thistask = (VoiceAnalyser *) SvbGetTask ( pid );

	thistask->followmouse = true;
	game->GetInterface ()->GetTaskManager ()->SetTargetProgram ( pid );

}

void VoiceAnalyser::PlayDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

	char name [64];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	VoiceAnalyser *thistask = (VoiceAnalyser *) SvbGetTask ( pid );

	if ( thistask->STATUS == VOICEANALYSER_STATUS_READY )
		imagebutton_draw ( button, highlighted, clicked );

}

void VoiceAnalyser::PlayClick ( Button *button )
{

	UplinkAssert (button);

	char name [64];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	VoiceAnalyser *thistask = (VoiceAnalyser *) SvbGetTask ( pid );

	if ( thistask->STATUS == VOICEANALYSER_STATUS_READY ) {

//#ifndef DEMOGAME

        Person *person = game->GetWorld ()->GetPerson ( thistask->personname );
        UplinkAssert (person);
        char filename [256];
        UplinkSnprintf ( filename, sizeof ( filename ), "sounds/analyser/verifyme%d.wav", person->voiceindex );
		SgPlaySound ( RsArchiveFileOpen ( filename ), filename, true );

//#endif
        
        thistask->STATUS = VOICEANALYSER_STATUS_REPRODUCING;
		thistask->timesync = (int) ( EclGetAccurateTime () + 8000 );

		char textbutton [64];
		UplinkSnprintf ( textbutton, sizeof ( textbutton ), "analyser_text %d", pid );
		EclRegisterCaptionChange ( textbutton, "Reproducing voice sample..." );

		// IF we're looking at a voice analysis screen,
		// input this voice

		if ( game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ()->ScreenID () == SCREEN_VOICEANALYSIS ) {
			
			VoiceAnalysisScreenInterface *vas = (VoiceAnalysisScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
			UplinkAssert (vas);

			vas->StartVoicePlayback ();

		}

	}

}

void VoiceAnalyser::PlayMouseMove ( Button *button )
{

	UplinkAssert (button);

	char name [64];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	VoiceAnalyser *thistask = (VoiceAnalyser *) SvbGetTask ( pid );

	if ( thistask->STATUS == VOICEANALYSER_STATUS_READY )
		button_highlight ( button );

}

void VoiceAnalyser::PlayMouseDown ( Button *button )
{

	UplinkAssert (button);

	char name [64];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	VoiceAnalyser *thistask = (VoiceAnalyser *) SvbGetTask ( pid );

	if ( thistask->STATUS == VOICEANALYSER_STATUS_READY )
		button_click ( button );

}

void VoiceAnalyser::DrawAnalysis ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

	char name [64];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	VoiceAnalyser *thistask = (VoiceAnalyser *) SvbGetTask ( pid );

	clear_draw ( button->x, button->y, button->width, button->height );

	glBegin ( GL_LINE_STRIP );

	for ( int i = 0; i < VOICEANALYSER_NUMSAMPLES; ++i ) {

		int x = button->x + i * (button->width/VOICEANALYSER_NUMSAMPLES);
		int y = (button->y + button->height) - thistask->sample [i] - 1;
		float r = 0.7f - ((float) thistask->sample[i] / 40.0f);		
		float b = (float) thistask->sample[i] / 40.0f;

		glColor4f ( r, 0.1f, b, 1.0f );
		glVertex2i ( x, y );

	}

	glEnd ();

	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
	border_draw ( button );

}

void VoiceAnalyser::MainTextDraw ( Button *button, bool highlighted, bool clicked )
{

	glBegin ( GL_QUADS );		
		glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x, button->y + button->height );
		glColor3ub ( 8, 20, 124 );		glVertex2i ( button->x, button->y );
		glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x + button->width, button->y );
		glColor3ub ( 8, 20, 124 );		glVertex2i ( button->x + button->width, button->y + button->height );
	glEnd ();

	glColor3ub ( 81, 138, 215 );
	border_draw ( button );

	text_draw ( button, highlighted, clicked );

}


void VoiceAnalyser::Initialise ()
{
}

void VoiceAnalyser::SetTarget ( UplinkObject *uo, char *uos, int uoi )
{

	if ( version < 2.0 ) return; // This function not available for version 1.0

   if ( STATUS == VOICEANALYSER_STATUS_WAITING ) {

      if ( uo->GetOBJECTID () == OID_DATABANK ) {

         /*
            DataBank selected

            uo  : DataBank object containing Data to be downloaded
            uos : Name of button representing downloadable data
            uoi : memory index of data in databank

            */

         if ( ((DataBank *) uo)->GetData (uoi) ) {

            source = (DataBank *) uo;
            sourceindex = uoi;

            Data *data = source->GetData (sourceindex);
            UplinkAssert (data);

            char ip[ SIZE_DATA_TITLE + 1 ];
            sscanf( data->title, "%s", ip );

            VLocation *loc = game->GetWorld ()->GetVLocation ( ip );
            if ( loc ) {

               Computer *comp = loc->GetComputer ();
               if ( comp && comp->TYPE == COMPUTER_TYPE_VOICEPHONESYSTEM ) {

                  STATUS = VOICEANALYSER_STATUS_NOTDOWNLOADING;

                  UplinkStrncpy ( personname, comp->companyname, sizeof ( personname ) );
                  UplinkStrncpy ( personip, comp->ip, sizeof ( personip ) );

                  numticksrequired = (int) ( TICKSREQUIRED_COPY * ((float) data->size / (float) game->GetWorld ()->GetPlayer ()->gateway.GetBandwidth ()) );
                  progress = 0;

                  remotefile = strstr ( uos, "fileserverscreen" ) ? true : false;

                  Button *button = EclGetButton ( uos );
                  UplinkAssert (button);

                  MoveTo ( button->x, button->y, 1000 );

               }
            }
         }
      }
   }
   else if ( STATUS == VOICEANALYSER_STATUS_READY ) {

      if ( uo->GetOBJECTID () == OID_DATABANK ) {

         Data *datacopy = new Data ();

         char datatitle[ max ( SIZE_VLOCATION_IP + 1 + SIZE_PERSON_NAME, SIZE_DATA_TITLE ) + 1 ];
         UplinkSnprintf ( datatitle, sizeof ( datatitle ), "%s %s", personip, personname );
         datatitle[ SIZE_DATA_TITLE - 1 ] = '\0';

         datacopy->SetTitle ( datatitle );
         datacopy->SetDetails ( DATATYPE_DATA, 1 );

         // Copying the file into a memory bank
         // Target memory area selected
         // If memory index is -1, look for a valid placement

         DataBank *db = (DataBank *) uo;
         int memoryindex = uoi;

         if ( memoryindex == -1 ) memoryindex = db->FindValidPlacement ( datacopy );

         if ( db->IsValidPlacement ( datacopy, memoryindex ) == 0 ) {
            
            db->PutData ( datacopy, memoryindex );
            STATUS = VOICEANALYSER_STATUS_FINISHED;

         }
      }
   } 
}

void VoiceAnalyser::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		int maxsample = 0;
	
		int pid = SvbLookupPID ( this );
		char textbutton [64];
		UplinkSnprintf ( textbutton, sizeof ( textbutton ), "analyser_text %d", pid );


		switch ( STATUS ) {

			case VOICEANALYSER_STATUS_WAITING:
			{

				maxsample = 10;

				// Are we looking at a voice phone screen?
				// If so, start recording the voice

				if ( game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ()->ScreenID () == SCREEN_VOICEPHONE ) {

					VoicePhoneScreenInterface *vps = (VoicePhoneScreenInterface *) (game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ());
					UplinkAssert (vps);
					
					if ( vps->STATUS == VOICESTATUS_TALKING ) {
						EclRegisterCaptionChange ( textbutton, "Recording voice pattern..." );
						STATUS = VOICEANALYSER_STATUS_RECORDING;
						UplinkStrncpy ( personname, vps->GetComputerScreen ()->GetComputer ()->companyname, sizeof ( personname ) );
						UplinkStrncpy ( personip, vps->GetComputerScreen ()->GetComputer ()->ip, sizeof ( personip ) );
					}

				}

			}
			break;
		
			case VOICEANALYSER_STATUS_RECORDING:
			{

				maxsample = 40;

				// We are now recording a voice
				// Until he finishes talking

				if ( game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ()->ScreenID () != SCREEN_VOICEPHONE ) {
				
					EclRegisterCaptionChange ( textbutton, "Awaiting voice input..." );
					STATUS = VOICEANALYSER_STATUS_WAITING;

				}

				VoicePhoneScreenInterface *vps = (VoicePhoneScreenInterface *) (game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ());
				UplinkAssert (vps);

				if ( vps->STATUS == VOICESTATUS_HUNGUP ) {
					EclRegisterCaptionChange ( textbutton, "Analysing voice recording..." );
					STATUS = VOICEANALYSER_STATUS_ANALYSING;
					timesync = (int) ( EclGetAccurateTime () + 5000 );
				}
				
			}
			break;

			case VOICEANALYSER_STATUS_ANALYSING:
			{

				maxsample = 10;

				if ( timesync > 0 && EclGetAccurateTime () > timesync ) {

					// Finished analysing
					EclRegisterCaptionChange ( textbutton, "Voice analysis complete.\nPlayback ready." );
					STATUS = VOICEANALYSER_STATUS_READY;
					timesync = 0;

					char play [64];
					UplinkSnprintf ( play, sizeof ( play ), "analyser_play %d", pid );				
					EclDirtyButton ( play );

				}

			}
			break;

			case VOICEANALYSER_STATUS_READY:
			{

				maxsample = 10;

			}
			break;

			case VOICEANALYSER_STATUS_REPRODUCING:
			{

				// We are currently playing back
				maxsample = 40;

				if ( timesync > 0 && EclGetAccurateTime () > timesync ) {

					// Finished playing back

					EclRegisterCaptionChange ( textbutton, "Finished playing back.\nPress play to repeat." );
					STATUS = VOICEANALYSER_STATUS_READY;
					timesync = 0;

					char play [64];
					UplinkSnprintf ( play, sizeof ( play ), "analyser_play %d", pid );				
					EclDirtyButton ( play );

					// IF we're looking at a voice analyser screen,
					// Tell it that we've finished playing our sample

					if ( game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ()->ScreenID () == SCREEN_VOICEANALYSIS ) {
						
						VoiceAnalysisScreenInterface *vas = (VoiceAnalysisScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
						UplinkAssert (vas);

						vas->FinishVoicePlayback ( personname );

					}

				}

			}
			break;

		  case VOICEANALYSER_STATUS_NOTDOWNLOADING:
		  {

			 // Not downloading - look for a new target

			 if ( source ) {

				// A new source file has been specified - start downloading it
				STATUS = VOICEANALYSER_STATUS_INPROGRESS;

				EclRegisterCaptionChange ( textbutton, "Downloading 0% ..." );

			 }

		  }
		  break;

		  case VOICEANALYSER_STATUS_INPROGRESS:
		  {

				// Check we are still connected
				if ( remotefile && !game->GetWorld ()->GetPlayer ()->IsConnected () ) {
					SvbRemoveTask (pid);
					return;
				}

			 // Download in progress
			 Data *data = source->GetData (sourceindex);   
				if ( !data ) {
					SvbRemoveTask(pid);
					return;
				}
	                     
			 for ( int count = 0; count < n; ++count ) {

				++progress;      
				char caption [64];
				UplinkSnprintf ( caption, sizeof ( caption ), "Downloading %d%% ...", (int)( ( (float) progress / (float) numticksrequired ) * 100 ) );
				EclRegisterCaptionChange ( textbutton, caption );

				if ( progress >= numticksrequired ) {

				   // Finished downloading now

				   EclRegisterCaptionChange ( textbutton, "Downloading complete.\nPlayback ready." );
				   STATUS = VOICEANALYSER_STATUS_READY;
				   timesync = 0;

				   char play [64];
				   UplinkSnprintf ( play, sizeof ( play ), "analyser_play %d", pid );            
				   EclDirtyButton ( play );

				   break;

				 }

			 }

		  }
		  break;

		  case VOICEANALYSER_STATUS_FINISHED:
		  {

			 STATUS = VOICEANALYSER_STATUS_READY;

		  }
		  break;

		}

		// ==================================================================================
		// Update the voice analyser image
		//

		if ( EclGetAccurateTime () > animsync + 75 ) {

			for ( int i = 0; i < VOICEANALYSER_NUMSAMPLES; ++i ) {

				sample [i] += NumberGenerator::RandomNumber ( maxsample / 2 ) - ( maxsample / 4 );
				if ( sample [i] < 0 ) sample [i] = 0;
				if ( sample [i] > maxsample ) sample [i] = maxsample;

			}
			
			char analyser [64];
			UplinkSnprintf ( analyser, sizeof ( analyser ), "analyser_analyser %d", pid );
			EclDirtyButton ( analyser );

			animsync = (int) EclGetAccurateTime ();

		}

		//	
		// ==================================================================================

	}

}

void VoiceAnalyser::MoveTo ( int x, int y, int time_ms )
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char title [64];
		char close [64];
		char text [64];
		char analyser [64];
		char play [64];

		UplinkSnprintf ( title, sizeof ( title ), "analyser_title %d", pid );
		UplinkSnprintf ( close, sizeof ( close ), "analyser_close %d", pid );
		UplinkSnprintf ( text, sizeof ( text ), "analyser_text %d", pid );
		UplinkSnprintf ( analyser, sizeof ( analyser ), "analyser_analyser %d", pid );
		UplinkSnprintf ( play, sizeof ( play ), "analyser_play %d", pid );

		EclRegisterMovement ( title, x + 1, y + 1, time_ms );
		EclRegisterMovement ( close, x + 276, y + 1, time_ms );
		EclRegisterMovement ( text, x + 1, y + 13, time_ms );
		EclRegisterMovement ( analyser, x + 188, y + 13, time_ms );
		EclRegisterMovement ( play, x + 170, y + 45, time_ms );

		EclButtonBringToFront ( title );
		EclButtonBringToFront ( close );
		EclButtonBringToFront ( text );
		EclButtonBringToFront ( analyser );
		EclButtonBringToFront ( play );

	}

}

void VoiceAnalyser::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char title [64];
		char close [64];
		char text [64];
		char analyser [64];
		char play [64];

		UplinkSnprintf ( title, sizeof ( title ), "analyser_title %d", pid );
		UplinkSnprintf ( close, sizeof ( close ), "analyser_close %d", pid );
		UplinkSnprintf ( text, sizeof ( text ), "analyser_text %d", pid );
		UplinkSnprintf ( analyser, sizeof ( analyser ), "analyser_analyser %d", pid );
		UplinkSnprintf ( play, sizeof ( play ), "analyser_play %d", pid );

		EclRegisterButton ( 200, 400, 275, 13, "Voice Analyser", "Click to move this application", title );
		EclRegisterButtonCallback ( title, TitleClick );
		
		EclRegisterButton ( 475, 400, 13, 13, "Close", "Close the analyser software", close );
		button_assignbitmaps ( close, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( close, CloseClick );
		
		if ( version < 2.0 )
			EclRegisterButton ( 200, 413, 188, 50, "Awaiting voice input...", "", text );
		else
			EclRegisterButton ( 200, 413, 188, 50, "Awaiting voice or data input...", "", text );
		EclRegisterButtonCallbacks ( text, MainTextDraw, NULL, NULL, NULL );

		EclRegisterButton ( 388, 413, 100, 50, " ", " ", analyser );
		EclRegisterButtonCallbacks ( analyser, DrawAnalysis, NULL, NULL, NULL );

		EclRegisterButton ( 370, 445, 15, 15, " ", "Play the analysed voice recording", play );
		button_assignbitmaps ( play, "software/play.tif", "software/play_h.tif", "software/play_c.tif" );
		EclRegisterButtonCallbacks ( play, PlayDraw, PlayClick, PlayMouseDown, PlayMouseMove );

		STATUS = VOICEANALYSER_STATUS_WAITING;

	}

}

void VoiceAnalyser::RemoveInterface ()
{
	
	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char title [64];
		char close [64];
		char text [64];
		char analyser [64];
		char play [64];

		UplinkSnprintf ( title, sizeof ( title ), "analyser_title %d", pid );
		UplinkSnprintf ( close, sizeof ( close ), "analyser_close %d", pid );
		UplinkSnprintf ( text, sizeof ( text ), "analyser_text %d", pid );
		UplinkSnprintf ( analyser, sizeof ( analyser ), "analyser_analyser %d", pid );
		UplinkSnprintf ( play, sizeof ( play ), "analyser_play %d", pid );

		EclRemoveButton ( title );
		EclRemoveButton ( close );
		EclRemoveButton ( text );
		EclRemoveButton ( analyser );
		EclRemoveButton ( play );

	}

}

void VoiceAnalyser::ShowInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char title [64];
		char close [64];
		char text [64];
		char analyser [64];
		char play [64];

		UplinkSnprintf ( title, sizeof ( title ), "analyser_title %d", pid );
		UplinkSnprintf ( close, sizeof ( close ), "analyser_close %d", pid );
		UplinkSnprintf ( text, sizeof ( text ), "analyser_text %d", pid );
		UplinkSnprintf ( analyser, sizeof ( analyser ), "analyser_analyser %d", pid );
		UplinkSnprintf ( play, sizeof ( play ), "analyser_play %d", pid );

		EclButtonBringToFront ( title );
		EclButtonBringToFront ( close );
		EclButtonBringToFront ( text );
		EclButtonBringToFront ( analyser );
		EclButtonBringToFront ( play );

	}

}

bool VoiceAnalyser::IsInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );
	char title [64];
	UplinkSnprintf ( title, sizeof ( title ), "analyser_title %d", pid );

	return ( EclGetButton ( title ) != NULL );

}


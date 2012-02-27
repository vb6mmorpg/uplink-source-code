
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


#include <stdio.h>

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "world/world.h"
#include "world/company/company.h"
#include "world/computer/computer.h"
#include "world/computer/computerscreen/genericscreen.h"
#include "world/generator/numbergenerator.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/voiceanalysisscreen_interface.h"

#include "mmgr.h"


VoiceAnalysisScreenInterface::VoiceAnalysisScreenInterface ()
{

	for ( int i = 0; i < VOICE_NUMSAMPLES; ++i )
		sample [i] = NumberGenerator::RandomNumber ( 40 );

	lastupdate = 0;

	STATUS = VOICEANALYSISSCREEN_NONE;

}

VoiceAnalysisScreenInterface::~VoiceAnalysisScreenInterface ()
{
}

void VoiceAnalysisScreenInterface::ClickAccess ( Button *button )
{

	VoiceAnalysisScreenInterface *thisint = (VoiceAnalysisScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);
	
	if ( thisint->STATUS == VOICEANALYSISSCREEN_PASSED ) {

		game->GetInterface ()->GetRemoteInterface ()->SetSecurity ( "Admin", 1 );
		int nextpage = thisint->GetComputerScreen ()->nextpage;
		if ( nextpage != -1 ) game->GetInterface ()->GetRemoteInterface ()->RunScreen ( nextpage, thisint->GetComputerScreen ()->GetComputer () );

	}

}

void VoiceAnalysisScreenInterface::StartVoicePlayback ()
{

	STATUS = VOICEANALYSISSCREEN_LISTENING;

	button_assignbitmap ( "voicescreen_access", "accessdenied.tif" );	

}

void VoiceAnalysisScreenInterface::FinishVoicePlayback ( char *voicename )
{

	STATUS = VOICEANALYSISSCREEN_ANALYSING;

	Company *company = game->GetWorld ()->GetCompany (GetComputerScreen ()->GetComputer ()->companyname);
	UplinkAssert (company);
	
	if ( strcmp ( voicename, company->administrator ) == 0 ) {

		STATUS = VOICEANALYSISSCREEN_PASSED;
		button_assignbitmap ( "voicescreen_access", "accessgranted.tif" );

	}
	else {

		STATUS = VOICEANALYSISSCREEN_FAILED;
		button_assignbitmap ( "voicescreen_access", "accessdenied.tif" );

	}

}

void VoiceAnalysisScreenInterface::DrawAnalysis ( Button *button, bool highlighted, bool clicked )
{

	VoiceAnalysisScreenInterface *thisint = (VoiceAnalysisScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

	clear_draw ( button->x, button->y, button->width, button->height );

#ifndef HAVE_GLES
	glBegin ( GL_LINE_STRIP );

	for ( int i = 0; i < VOICE_NUMSAMPLES; ++i ) {

		int x = button->x + i * (button->width/VOICE_NUMSAMPLES);
		int y = (button->y + button->height) - thisint->sample [i];
		float c = (float) thisint->sample[i] / 40.0f;

		glColor4f ( 0.1f, 0.1f, c, 1.0f );
		glVertex2i ( x, y );

	}

	glEnd ();
#else
	GLfloat verts[VOICE_NUMSAMPLES * 2];
	GLfloat colors[VOICE_NUMSAMPLES * 4];

	for ( int i = 0; i < VOICE_NUMSAMPLES; ++i ) {
		int x = button->x + i * (button->width/VOICE_NUMSAMPLES);
		int y = (button->y + button->height) - thisint->sample [i];
		float c = (float) thisint->sample[i] / 40.0f;

		verts[i*2] = x;
		verts[i*2+1] = y;
		colors[i*2] = 0.1f;
		colors[i*2+1] = 0.1f;
		colors[i*2+2] = c;
		colors[i*2+3] = 1.0f;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, verts);
	glColorPointer(4, GL_FLOAT, 0, colors);
	glDrawArrays(GL_LINE_STRIP, 0, VOICE_NUMSAMPLES);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#endif

	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
	border_draw ( button );

}

void VoiceAnalysisScreenInterface::DrawBackground ( Button *button, bool highlighted, bool clicked )
{

#ifndef HAVE_GLES
	glBegin ( GL_QUADS );		
		glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x, button->y + button->height );
		glColor3ub ( 8, 20, 124 );		glVertex2i ( button->x, button->y );
		glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x + button->width, button->y );
		glColor3ub ( 8, 20, 124 );		glVertex2i ( button->x + button->width, button->y + button->height );
	glEnd ();
#else
	GLfloat verts[] = {
		button->x, button->y + button->height,
		button->x, button->y,
		button->x + button->width, button->y,
		button->x + button->width, button->y + button->height
	};

	GLubyte colors[] = {
		8, 20, 0, 255,
		8, 20, 124, 255,
		8, 20, 0, 255,
		8, 20, 124, 255
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, verts);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif

	glColor4ub ( 81, 138, 215, 255 );
	border_draw ( button );
	
}

bool VoiceAnalysisScreenInterface::ReturnKeyPressed ()
{

	return false;

}

void VoiceAnalysisScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert (newcs);
	cs = newcs;

	if ( !IsVisible () ) {

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "voicescreen_maintitle" );
		EclRegisterButtonCallbacks ( "voicescreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "voicescreen_subtitle" );
		EclRegisterButtonCallbacks ( "voicescreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

		EclRegisterButton ( 50, 130, 400, 15, " ", " ", "voicescreen_instructions" );
		EclRegisterButtonCallbacks ( "voicescreen_instructions", textbutton_draw, NULL, NULL, NULL );
		// used to be "Please read the following sentence clearly into your microphone."
		EclRegisterCaptionChange ( "voicescreen_instructions", "Please read the following sentence into your voice device.", 0 );

		EclRegisterButton ( 50, 180, 380, 150, " ", " ", "voicescreen_background" );
		EclRegisterButtonCallbacks ( "voicescreen_background", DrawBackground, NULL, NULL, NULL );

		EclRegisterButton ( 75, 225, 100, 50, " ", " ", "voicescreen_analysis" );
		EclRegisterButtonCallbacks ( "voicescreen_analysis", DrawAnalysis, NULL, NULL, NULL );

		EclRegisterButton ( 185, 225, 250, 50, " ", " ", "voicescreen_sentence" );
		EclRegisterButtonCallbacks ( "voicescreen_sentence", text_draw, NULL, NULL, NULL );
		EclRegisterCaptionChange ( "voicescreen_sentence", "Hello.  I am the system administrator.\nMy voice is my passport.\nVerify me." );

		EclRegisterButton ( 300, 300, 100, 20, " ", " ", "voicescreen_access" );
		button_assignbitmap ( "voicescreen_access", "accessdenied.tif" );
		EclRegisterButtonCallback ( "voicescreen_access", ClickAccess );

	}

}

void VoiceAnalysisScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "voicescreen_maintitle" );
		EclRemoveButton ( "voicescreen_subtitle" );

		EclRemoveButton ( "voicescreen_background" );

		EclRemoveButton ( "voicescreen_instructions" );
		EclRemoveButton ( "voicescreen_analysis" );
		EclRemoveButton ( "voicescreen_sentence" );
		
		EclRemoveButton ( "voicescreen_access" );

	}

}

void VoiceAnalysisScreenInterface::Update ()
{

	int maxvalue = 0;

	switch ( STATUS ) {

		case VOICEANALYSISSCREEN_NONE:
		{
	
			maxvalue = 10;

		}
		break;

		case VOICEANALYSISSCREEN_LISTENING:
		{

			maxvalue = 40;

		}
		break;

		default:
			
			maxvalue = 10;
			break;

	}

	// Update the voice analysis box

	if ( EclGetAccurateTime () > lastupdate + 75 ) {

		for ( int i = 0; i < VOICE_NUMSAMPLES; ++i ) {

			sample [i] += NumberGenerator::RandomNumber ( maxvalue / 2 ) - ( maxvalue / 4 );
			if ( sample [i] < 0 ) sample [i] = 0;
			if ( sample [i] > maxvalue ) sample [i] = maxvalue;

		}
		
		EclDirtyButton ( "voicescreen_analysis" );

		lastupdate = (int) EclGetAccurateTime ();

	}

}

bool VoiceAnalysisScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "voicescreen_maintitle" ) != NULL );

}

int VoiceAnalysisScreenInterface::ScreenID ()
{

	return SCREEN_VOICEANALYSIS;

}

GenericScreen *VoiceAnalysisScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);
	return (GenericScreen *) cs;

}

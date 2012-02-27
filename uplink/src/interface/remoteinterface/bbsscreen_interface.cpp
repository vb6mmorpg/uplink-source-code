
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

#include "eclipse.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "game/game.h"

#include "interface/interface.h"
#include "interface/scrollbox.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/bbsscreen_interface.h"
#include "interface/remoteinterface/contactscreen_interface.h"

#include "world/computer/computerscreen/bbsscreen.h"

#include "world/world.h"
#include "world/player.h"
#include "world/company/mission.h"
#include "world/company/companyuplink.h"
#include "world/generator/missiongenerator.h"

#include "mmgr.h"


// === Initialise static variables ============================================


int BBSScreenInterface::baseoffset = 0;
int BBSScreenInterface::currentselect = -1;
int BBSScreenInterface::previousnummessages = 0;
int BBSScreenInterface::previousupdate = 0;

// ============================================================================


void BBSScreenInterface::ClickBBSButton ( Button *button )
{

	int index;
	sscanf ( button->name, "BBmessage %d", &index );

	index += baseoffset;

	// Dirty the old button
	char oldname [128];
	UplinkSnprintf ( oldname, sizeof ( oldname ), "BBmessage %d", currentselect - baseoffset );
	EclDirtyButton ( oldname );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	if ( cu->GetMission ( index ) ) {

		currentselect = index;

		if ( game->GetWorld ()->GetPlayer ()->rating.uplinkrating >= cu->GetMission (index)->minuplinkrating ) 
			EclRegisterCaptionChange ( "bbs_details", cu->GetMission (index)->GetDetails (), 2000 );

		else
			EclRegisterCaptionChange ( "bbs_details", "You do not have a sufficient Uplink Rating to view this message.", 2000 );

	}
	
}

void BBSScreenInterface::DrawBBSButton ( Button *button, bool highlighted, bool clicked )
{

	int index;
	sscanf ( button->name, "BBmessage %d", &index );
	index += baseoffset;

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

	clear_draw ( button->x, button->y, button->width, button->height );

	// Get the text from mission number (index + baseoffset)

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	Mission *mission = cu->GetMission ( index );

	if ( mission ) {
		
		if ( index == currentselect ) {

#ifndef HAVE_GLES
			glBegin ( GL_QUADS );
				SetColour ( "PanelHighlightA" );        glVertex2i ( button->x, button->y );
				SetColour ( "PanelHighlightB" );        glVertex2i ( button->x + button->width, button->y );
				SetColour ( "PanelHighlightA" );        glVertex2i ( button->x + button->width, button->y + button->height );
				SetColour ( "PanelHighlightB" );        glVertex2i ( button->x, button->y + button->height );
			glEnd ();
#else
			ColourOption *col1, *col2;
			col1 = GetColour("PanelHighlightA");
			col2 = GetColour("PanelHighlightB");
			GLfloat verts[] = {
				button->x, button->y,
				button->x + button->width, button->y,
				button->x + button->width, button->y + button->height,
				button->x, button->y + button->height
			};

			GLfloat colors[] = {
				col1->r, col1->g, col1->b, 1.0f,
				col2->r, col2->g, col2->b, 1.0f,
				col1->r, col1->g, col1->b, 1.0f,
				col2->r, col2->g, col2->b, 1.0f
			};

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);

			glVertexPointer(2, GL_FLOAT, 0, verts);
			glColorPointer(4, GL_FLOAT, 0, colors);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
#endif
		}

		if ( highlighted ) {

			SetColour ( "PanelHighlightBorder" );
			border_draw ( button );

		}

		char date [64], subject [256];
		bool encrypted = ( game->GetWorld ()->GetPlayer ()->rating.uplinkrating < mission->minuplinkrating );

		if ( !encrypted ) {

			float ratingdif = (float)(game->GetWorld ()->GetPlayer ()->rating.uplinkrating - mission->minuplinkrating);
			if ( ratingdif > 5.0f ) ratingdif = 5.0f;
			
			UplinkStrncpy ( subject, mission->description, sizeof ( subject ) );
			UplinkStrncpy ( date, mission->createdate.GetShortString (), sizeof ( date ) );

#ifdef DEMOGAME
            glColor4f ( 1.0f - ratingdif * 0.2f, 1.0f - ratingdif * 0.2f, 1.0f - ratingdif * 0.2f, 1.0f );
#else
			glColor4f ( 1.0f - ratingdif * 0.1f, 1.0f - ratingdif * 0.1f, 1.0f - ratingdif * 0.1f, 1.0f );
#endif

		}
		else {

			UplinkStrncpy ( subject, "Encrypted (Insufficient Uplink Rating)", sizeof ( subject ) );
			UplinkStrncpy ( date, "Unknown", sizeof ( date ) );

			glColor4f ( 0.2f, 0.2f, 0.2f, 1.0f );		

		}

		GciDrawText ( button->x + 110, button->y + 10, subject );
		GciDrawText ( button->x + 5, button->y + 10, date );

	}

	glDisable ( GL_SCISSOR_TEST );

}

void BBSScreenInterface::DrawDetails ( Button *button, bool highlighted, bool clicked )
{

#ifndef HAVE_GLES
	glBegin ( GL_QUADS );
		SetColour ( "PanelBackgroundA" );       glVertex2i ( button->x, button->y + button->height );
		SetColour ( "PanelBackgroundB" );       glVertex2i ( button->x, button->y );
		SetColour ( "PanelBackgroundA" );       glVertex2i ( button->x + button->width, button->y );
		SetColour ( "PanelBackgroundB" );       glVertex2i ( button->x + button->width, button->y + button->height );
	glEnd ();
#else
        ColourOption *col1, *col2;
        col1 = GetColour("PanelBackgroundA");
        col2 = GetColour("PanelBackgroundB");
        GLfloat verts[] = {
                button->x, button->y + button->height,
                button->x, button->y,
                button->x + button->width, button->y,
                button->x + button->width, button->y + button->height
        };

        GLfloat colors[] = {
                col1->r, col1->g, col1->b, 1.0f,
                col2->r, col2->g, col2->b, 1.0f,
                col1->r, col1->g, col1->b, 1.0f,
                col2->r, col2->g, col2->b, 1.0f
        };

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(2, GL_FLOAT, 0, verts);
        glColorPointer(4, GL_FLOAT, 0, colors);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
#endif

    SetColour ( "PanelBorder" );
	border_draw ( button );

	text_draw ( button, highlighted, clicked );

}

void BBSScreenInterface::MousedownBBSButton ( Button *button )
{

	int index;
	sscanf ( button->name, "BBmessage %d", &index );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	if ( cu->GetMission ( index + baseoffset ) ) 
		button_click ( button );
	

}

void BBSScreenInterface::HighlightBBSButton ( Button *button )
{

	int index;
	sscanf ( button->name, "BBmessage %d", &index );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	if ( cu->GetMission ( index + baseoffset ) )
		button_highlight ( button );

}

void BBSScreenInterface::ExitClick ( Button *button )
{

	BBSScreen *bbs = (BBSScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (bbs);

	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( bbs->nextpage, bbs->GetComputer () );

}

void BBSScreenInterface::AcceptClick ( Button *button )
{

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	Mission *mission = cu->GetMission ( currentselect );

	if ( mission ) {

		// Can we auto-accept this mission?

		if ( mission->acceptrating == -1 ) {

			EclRegisterCaptionChange ( "bbs_details", "This mission cannot be auto-accepted.  You must contact the employers"
													  "to accept this mission." );

			return;

		}

		// There are some cases where a mission cannot be accepted
		// -eg trace a hacker who is in fact the player
		// This should be duplicated in the contact button code

		if ( mission->TYPE == MISSION_TRACEUSER &&
			 strcmp ( mission->completionA, "PLAYER" ) == 0 ) {

			EclRegisterCaptionChange ( "bbs_details", "We cannot allow you to accept this mission, on the specific "
													  "instructions of the employer." );

			return;

		}

		if ( game->GetWorld ()->GetPlayer ()->rating.uplinkrating >= mission->acceptrating ) {

			game->GetWorld ()->GetPlayer ()->GiveMission ( mission );
		
			cu->missions.RemoveData ( currentselect );
		
			currentselect = -1;
			EclRegisterCaptionChange ( "bbs_details", " " );

		}		
		else {

			EclRegisterCaptionChange ( "bbs_details", "You do not have a sufficient Uplink Rating to auto-accept this mission."
													  "You must first contact the employers." );

		}

	}

}

void BBSScreenInterface::ContactClick ( Button *button )
{

	// Get the currently selected mission

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	Mission *mission = cu->GetMission ( currentselect );

	// Get the bbs screen

	BBSScreen *bbs = (BBSScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (bbs);
	
	if ( mission && bbs->contactpage != -1 ) {

		// There are some cases where a mission cannot be dealt with
		// -eg trace a hacker who is in fact the player
		// This code is repeated in the accept button code

		if ( mission->TYPE == MISSION_TRACEUSER &&
			 strcmp ( mission->completionA, "PLAYER" ) == 0 ) {

			EclRegisterCaptionChange ( "bbs_details", "We cannot allow you to contact this employer, "
													  "on their specific instructions." );

			return;

		}

		if ( game->GetWorld ()->GetPlayer ()->rating.uplinkrating >= mission->minuplinkrating ) {

			game->GetInterface ()->GetRemoteInterface ()->RunScreen ( bbs->contactpage, bbs->GetComputer () );
			
			ContactScreenInterface *cs = (ContactScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
			
			UplinkAssert ( cs );
			UplinkAssert ( cs->ScreenID () == SCREEN_CONTACTSCREEN );

			cs->SetTYPE ( CONTACT_TYPE_MISSION );
			cs->SetMission ( mission );

		}
		else {

			EclRegisterCaptionChange ( "bbs_details", "You do not have a sufficient Uplink Rating to contact this employer." );

		}

	}

}

void BBSScreenInterface::ScrollChange ( char *scrollname, int newValue )
{

    baseoffset = newValue;

	for ( int i = 0; i < NumItemsOnScreen(); ++i ) {

		char name [128];
		UplinkSnprintf ( name, sizeof ( name ), "BBmessage %d", i );
		EclDirtyButton ( name );

	}

}

bool BBSScreenInterface::EscapeKeyPressed ()
{

    ExitClick (NULL);    
    return true;

}

int BBSScreenInterface::NumItemsOnScreen ()
{
    int screenheight = app->GetOptions()->GetOptionValue( "graphics_screenheight" );
    int availablePixels = screenheight - 200 - 45;
    return availablePixels/20;
}

void BBSScreenInterface::Create ( ComputerScreen *newcs )
{

	if ( !IsVisible () ) {

		UplinkAssert ( newcs );
		cs = newcs;

		EclRegisterButton ( 20, 30, 100, 15, "Date", "This column shows the date the message was submitted", "bbs_date" );
		EclRegisterButton ( 125, 30, 22 + SY(388) - 125 + 15, 15, "Subject", "This column shows the subject of the message", "bbs_subject" );

		for ( int i = 0; i < NumItemsOnScreen(); ++i ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "BBmessage %d", i );
			EclRegisterButton ( 20, i * 20 + 50, SY(388), 17, "", "Read this Bulletin Board message", name );
			EclRegisterButtonCallbacks ( name, DrawBBSButton, ClickBBSButton, MousedownBBSButton, HighlightBBSButton ); 
						
		}
		
        int baseX = 23 + SY(388) - 55;
        int baseY = 50 + NumItemsOnScreen() * 20;

        EclRegisterButton ( 20, baseY, SY(388) - 55, 110, "", "", "bbs_details" );
		EclRegisterButtonCallbacks ( "bbs_details", DrawDetails, NULL, NULL, NULL );

		EclRegisterButton ( baseX, baseY, 70, 15, "Accept", "Accept this mission", "bbs_accept" );
		EclRegisterButtonCallback ( "bbs_accept", AcceptClick );

		EclRegisterButton ( baseX, baseY + 20, 70, 15, "Contact", "Contact the employer of this mission", "bbs_contact" );
		EclRegisterButtonCallback ( "bbs_contact", ContactClick );

		EclRegisterButton ( baseX, baseY + 110 - 15, 70, 15, "Exit BBS", "Close the BBS screen and return to the main menu", "bbs_exit" );
		EclRegisterButtonCallback ( "bbs_exit", ExitClick );

    	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	    UplinkAssert ( cu );
        ScrollBox::CreateScrollBox ( "bbs_scroll", 22 + SY(388), 47, 15, NumItemsOnScreen() * 20, cu->missions.Size(), 12, 0, ScrollChange );

		baseoffset = 0;
		currentselect = -1;

	}


}

void BBSScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		for ( int i = 0; i < NumItemsOnScreen(); ++ i ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "BBmessage %d", i );

			EclRemoveButton ( name );

		}

		EclRemoveButton ( "bbs_details" );

		EclRemoveButton ( "bbs_date" );
		EclRemoveButton ( "bbs_subject" );
		
//		EclRemoveButton ( "bbs_scrollup" );
//		EclRemoveButton ( "bbs_scrolldown" );
//		EclRemoveButton ( "bbs_scrollbar" );

		EclRemoveButton ( "bbs_accept" );
		EclRemoveButton ( "bbs_contact" );

		EclRemoveButton ( "bbs_exit" );

        ScrollBox::RemoveScrollBox( "bbs_scroll" );

	}

}

bool BBSScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "bbs_details" ) != 0 );	

}

void BBSScreenInterface::Update ()
{

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	int newnummessages = cu->missions.Size ();
    if ( newnummessages != previousnummessages ) {
        ScrollBox::GetScrollBox( "bbs_scroll" )->SetNumItems( cu->missions.Size() );
		previousnummessages = newnummessages;
    }

	if ( time(NULL) > previousupdate ) {

		for ( int i = 0; i < NumItemsOnScreen(); ++ i ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "BBmessage %d", i );
			EclDirtyButton ( name );

		}
		
		previousupdate = (int) ( time(NULL) + 1 );

	}

}

int BBSScreenInterface::ScreenID ()
{

	return SCREEN_BBSSCREEN;

}

BBSScreen *BBSScreenInterface::GetComputerScreen ()
{

	UplinkAssert ( cs );
	return (BBSScreen *) cs;

}

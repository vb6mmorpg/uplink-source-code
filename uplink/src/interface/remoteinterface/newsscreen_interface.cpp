
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
#include "interface/remoteinterface/newsscreen_interface.h"
#include "interface/remoteinterface/contactscreen_interface.h"

#include "world/computer/computerscreen/genericscreen.h"

#include "world/world.h"
#include "world/player.h"
#include "world/company/news.h"
#include "world/company/companyuplink.h"

#include "mmgr.h"

// === Initialise static variables ============================================


int NewsScreenInterface::baseoffset = 0;
int NewsScreenInterface::currentselect = -1;
int NewsScreenInterface::previousnummessages = 0;


// ============================================================================


void NewsScreenInterface::ClickNewsButton ( Button *button )
{

	int index;
	sscanf ( button->name, "news_story %d", &index );

	index += baseoffset;

	// Dirty the old button

	char oldname [128];
	UplinkSnprintf ( oldname, sizeof ( oldname ), "news_story %d", currentselect - baseoffset );
	EclDirtyButton ( oldname );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	if ( cu->GetNews (index) ) {

		currentselect = index;
        
	    // Reset the offset so the player can read it from line 1
        ScrollBox *scrollBox = ScrollBox::GetScrollBox( "news_details" );
        if ( scrollBox ) {
            scrollBox->SetCurrentIndex( 0 );

            char *newDetails = cu->GetNews (index)->GetDetails();
            Button *detailsButton = EclGetButton ( "news_details box" );
            UplinkAssert (detailsButton);
        	LList <char *> *wrappedtext = wordwraptext ( newDetails, detailsButton->width );
			if ( wrappedtext ) {
				scrollBox->SetNumItems( wrappedtext->Size() );
				if ( wrappedtext->ValidIndex (0) && wrappedtext->GetData (0) )
					delete [] wrappedtext->GetData(0);
				delete wrappedtext;
			}
			else {
				scrollBox->SetNumItems( 0 );
			}
        }

		EclRegisterCaptionChange ( "news_details box", cu->GetNews (index)->GetDetails (), 2000 );

	}
	
}

void NewsScreenInterface::DrawNewsButton ( Button *button, bool highlighted, bool clicked )
{

	int index;
	sscanf ( button->name, "news_story %d", &index );
	index += baseoffset;

	clear_draw ( button->x, button->y, button->width, button->height );

	// Get the text from news item (index + baseoffset)

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	News *news = cu->GetNews (index);

	if ( news ) {

		int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
		glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
		glEnable ( GL_SCISSOR_TEST );

		ColourOption *col1, *col2;
		if ( index == currentselect ) {

#ifndef HAVE_GLES
			glBegin ( GL_QUADS );
				SetColour ( "PanelHighlightA" );		glVertex2i ( button->x, button->y );
				SetColour ( "PanelHighlightB" );        glVertex2i ( button->x + button->width, button->y );
				SetColour ( "PanelHighlightA" );        glVertex2i ( button->x + button->width, button->y + button->height );
				SetColour ( "PanelHighlightB" );        glVertex2i ( button->x, button->y + button->height );
			glEnd ();
#else
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
		else {
			
#ifndef HAVE_GLES
			if ( index % 2 == 0 ) {

				glBegin ( GL_QUADS );
					SetColour ( "DarkPanelA" );     glVertex2i ( button->x, button->y + button->height );
					SetColour ( "DarkPanelB" );     glVertex2i ( button->x, button->y );
					SetColour ( "DarkPanelA" );     glVertex2i ( button->x + button->width, button->y );
					SetColour ( "DarkPanelB" );     glVertex2i ( button->x + button->width, button->y + button->height );
				glEnd ();

			}
			else {

				glBegin ( GL_QUADS );
					SetColour ( "DarkPanelB" );     glVertex2i ( button->x, button->y + button->height );
					SetColour ( "DarkPanelA" );     glVertex2i ( button->x, button->y );
					SetColour ( "DarkPanelB" );     glVertex2i ( button->x + button->width, button->y );
					SetColour ( "DarkPanelA" );     glVertex2i ( button->x + button->width, button->y + button->height );
				glEnd ();

			}
#else
			if (index % 2 == 0) {
				col1 = GetColour("DarkPanelA");
				col2 = GetColour("DarkPanelB");
			} else {
				col1 = GetColour("DarkPanelA");
				col2 = GetColour("DarkPanelB");
			}

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

		}

		if ( highlighted ) {

			SetColour ( "PanelHighlightBorder" );
			border_draw ( button );

		}

        // Get the date and subject

		char date [64];
        char subject [256];        
		UplinkStrncpy ( subject, news->headline, sizeof ( subject ) );
		UplinkStrncpy ( date, news->date.GetShortString (), sizeof ( date ) );

        // Get the first line of the news story

        char *fulldetails = news->GetDetails ();
        char *firstendline = strchr ( fulldetails, '\n' );
        if ( firstendline && firstendline - fulldetails > 255 )
            firstendline = fulldetails + 255;
        char details [256];
        if ( firstendline ) {
            strncpy ( details, news->GetDetails (), ( firstendline - fulldetails ) + 1 );
            details[firstendline - fulldetails] = '\x0';                            // Added by Contraband
        }
        else
            UplinkStrncpy ( details, " ", sizeof ( details ) );

        // Draw the text items

		SetColour ( "DefaultText" );
		GciDrawText ( button->x + 110, button->y + 10, subject );
		GciDrawText ( button->x + 5, button->y + 10, date );

        SetColour ( "DimmedText" );
        GciDrawText ( button->x + 110, button->y + 25, details );

		glDisable ( GL_SCISSOR_TEST );

	}
	
}

void NewsScreenInterface::DrawDetails ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

	// Get the offset

	char name_base [128];
	sscanf ( button->name, "%s", name_base );
    ScrollBox *scrollBox = ScrollBox::GetScrollBox( name_base );
    if ( !scrollBox ) return;
    int offset = scrollBox->currentIndex;
	
	// Draw the button

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


	// Draw the text

	int maxnumlines = (button->height - 10 ) / 15;

	SetColour ( "DefaultText" );

	LList <char *> *wrappedtext = wordwraptext ( button->caption, button->width );

	if ( wrappedtext ) {

		for ( int i = offset; i < wrappedtext->Size (); ++i ) {

			if ( i > maxnumlines + offset )
				break;

			int xpos = button->x + 10;
			int	ypos = button->y + 10 + (i-offset) * 15;

			GciDrawText ( xpos, ypos, wrappedtext->GetData (i), HELVETICA_10 );

		}

		//DeleteLListData ( wrappedtext );							// Only delete first entry - since there is only one string really
		if ( wrappedtext->ValidIndex (0) && wrappedtext->GetData (0) )
			delete [] wrappedtext->GetData (0);
		delete wrappedtext;

	}

	glDisable ( GL_SCISSOR_TEST );
	
}

void NewsScreenInterface::MousedownNewsButton ( Button *button )
{

	int index;
	sscanf ( button->name, "news_story %d", &index );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	if ( cu->GetNews ( index + baseoffset ) ) 
		button_click ( button );
	

}

void NewsScreenInterface::HighlightNewsButton ( Button *button )
{

	int index;
	sscanf ( button->name, "news_story %d", &index );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	if ( cu->GetNews ( index + baseoffset ) )
		button_highlight ( button );

}

void NewsScreenInterface::ExitClick ( Button *button )
{

	GenericScreen *gs = (GenericScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (gs);
	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( gs->nextpage, gs->GetComputer () );

}

bool NewsScreenInterface::EscapeKeyPressed ()
{

    ExitClick (NULL);
    return true;

}


void NewsScreenInterface::ScrollChange ( char *scrollname, int newValue )
{

    baseoffset = newValue;

    int numRows = SY(260) / 40;

	for ( int i = 0; i < numRows; ++i ) {

		char name [128];
		UplinkSnprintf ( name, sizeof ( name ), "news_story %d", i );
		EclDirtyButton ( name );

	}

}

void NewsScreenInterface::Create ( ComputerScreen *newcs )
{

	if ( !IsVisible () ) {

		UplinkAssert ( newcs );
		cs = newcs;

        int numRows = SY(260) / 40;
        int itemWidth = SX(390);

		EclRegisterButton ( 20, 34, 104, 15, "Date", "This column shows the date the story was posted", "news_date" );
		EclRegisterButton ( 125, 34, itemWidth - 105 + 15, 15, "Subject", "This column shows the subject of the story", "news_subject" );

		for ( int i = 0; i < numRows; ++i ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "news_story %d", i );
			EclRegisterButton ( 20, i * 40 + 50, itemWidth, 39, "", "Read this News story", name );
			EclRegisterButtonCallbacks ( name, DrawNewsButton, ClickNewsButton, MousedownNewsButton, HighlightNewsButton ); 
						
		}
		
		create_stextbox ( 20, 50 + numRows * 40, itemWidth + 16, SY(105), "", "news_details" );
		EclRegisterButtonCallbacks ( "news_details box", DrawDetails, NULL, NULL, NULL);

		EclRegisterButton ( 20 + itemWidth + 16 - SX(70), 51 + numRows * 40 + SY(105), SX(70), 15, "Exit News", "Close the News screen and return to the main menu", "news_exit" );
		EclRegisterButtonCallback ( "news_exit", ExitClick );

    	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	    UplinkAssert ( cu );
        ScrollBox::CreateScrollBox( "news_scroll", itemWidth + 21, 50, 15, numRows * 40, cu->news.Size(), numRows, 0, ScrollChange );

		baseoffset = 0;
		currentselect = -1;

	}

}

void NewsScreenInterface::Remove ()
{

	if ( IsVisible () ) {

        int numRows = SY(260) / 40;

		for ( int i = 0; i < numRows; ++ i ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "news_story %d", i );

			EclRemoveButton ( name );

		}

		remove_stextbox ( "news_details" );

		EclRemoveButton ( "news_date" );
		EclRemoveButton ( "news_subject" );

		EclRemoveButton ( "news_exit" );

        ScrollBox::RemoveScrollBox ( "news_scroll" );

	}

}

bool NewsScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "news_exit" ) != 0 );

}

void NewsScreenInterface::Update ()
{

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	int newnummessages = cu->news.Size ();

	if ( newnummessages != previousnummessages ) {

        int numRows = SY(260) / 40;

		for ( int i = 0; i < numRows; ++ i ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "news_story %d", i );
			EclDirtyButton ( name );

		}

        ScrollBox::GetScrollBox( "news_scroll" )->SetNumItems( newnummessages );
		previousnummessages = newnummessages;

	}

}

int NewsScreenInterface::ScreenID ()
{

	return SCREEN_NEWSSCREEN;

}

GenericScreen *NewsScreenInterface::GetComputerScreen ()
{

	UplinkAssert ( cs );
	return (GenericScreen *) cs;

}


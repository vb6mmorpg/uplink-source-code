
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

#include "eclipse.h"
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/miscutils.h"

#include "game/game.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/shareslistscreen_interface.h"
#include "interface/remoteinterface/sharesviewscreen_interface.h"
#include "interface/localinterface/phonedialler.h"

#include "world/world.h"
#include "world/message.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/computerscreen/shareslistscreen.h"

#include "mmgr.h"


int SharesListScreenInterface::baseoffset = 0;


void SharesListScreenInterface::ShareClick ( Button *button )
{

	UplinkAssert ( button );

	int fileindex;
	sscanf ( button->name, "shareslistscreen_share %d", &fileindex );
	fileindex += baseoffset;

	SharesListScreenInterface *thisint = (SharesListScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

	if ( thisint->filteredlist.ValidIndex (fileindex) ) {

		char *companyname = thisint->filteredlist.GetData (fileindex);

		game->GetInterface ()->GetRemoteInterface ()->RunScreen ( thisint->GetComputerScreen ()->viewpage, thisint->GetComputerScreen ()->GetComputer () );
		SharesViewScreenInterface *newint = (SharesViewScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
		UplinkAssert (newint);
		newint->SetCompanyName ( companyname );

	}

}

void SharesListScreenInterface::ShareDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert ( button );

	clear_draw ( button->x, button->y, button->width, button->height );

	int shareindex;
	sscanf ( button->name, "shareslistscreen_share %d", &shareindex );
	shareindex += baseoffset;

	LList <char *> *filteredlist = &((SharesListScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ())->filteredlist;
	char *companyname = filteredlist->GetData (shareindex);

	/*
		Re above line

		calling getdata like that will be pretty slow, since GetData on a linked list 
		has to iterate accross the entire array.

		But this call of ShareDraw will come after another call to ShareDraw, for the share
		at shareindex-1.  This means that sequential access is occuring - which will be ok
		in terms of speed.

    */

	if ( companyname ) {

		Company *company = game->GetWorld ()->GetCompany ( companyname );
		UplinkAssert (company);

#ifndef HAVE_GLES
		if ( shareindex % 2 == 0 ) {

			glBegin ( GL_QUADS );
				glColor3ub ( 8, 20, 80 );		glVertex2i ( button->x, button->y );
				glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x + button->width, button->y );
				glColor3ub ( 8, 20, 80 );		glVertex2i ( button->x + button->width, button->y + button->height );
				glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x, button->y + button->height );
			glEnd ();

		}
		else {

			glBegin ( GL_QUADS );
				glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x, button->y );
				glColor3ub ( 8, 20, 80 );		glVertex2i ( button->x + button->width, button->y );
				glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x + button->width, button->y + button->height );
				glColor3ub ( 8, 20, 80 );		glVertex2i ( button->x, button->y + button->height );
			glEnd ();

		}
#else
		GLfloat verts[] = {
			button->x, button->y,
			button->x + button->width, button->y,
			button->x + button->width, button->y + button->height,
			button->x, button->y + button->height
		};

		GLubyte colors[] = {
			8, 20, 80, 255,
			8, 20, 0, 255,
			8, 20, 80, 255,
			8, 20, 0, 255,
			8, 20, 80, 255
		};

		glVertexPointer(2, GL_FLOAT, 0, verts);

		if (shareindex % 2 == 0) {
			glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
		} else {
			glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors+4);
		}

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
#endif

		glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );

		char currentprice [16];
		UplinkSnprintf ( currentprice, sizeof ( currentprice ), "%d c", company->GetSharePrice () );

		char changeinprice [16];
		if ( company->GetShareChange () != 0 ) {
			UplinkSnprintf ( changeinprice, sizeof ( changeinprice ), "(%d%%)", company->GetShareChange () );
		} else {
			UplinkStrncpy ( changeinprice, "-", sizeof ( changeinprice ) );
		}

        // If we own any, put a * by the name

        if ( game->GetInterface ()->GetRemoteInterface ()->security_level < 10 &&
             game->GetWorld ()->GetPlayer ()->NumSharesOwned ( companyname ) > 0 ) 
            GciDrawText ( button->x + 10, button->y + 10, "*" );

		// Draw the text

		GciDrawText ( button->x + 20, button->y + 10, company->name );
		GciDrawText ( button->x + 200, button->y + 10, currentprice );

		if ( company->GetShareChange () < 0 ) glColor4f ( 1.0f, 0.0f, 0.0f, 1.0f );
		GciDrawText ( button->x + 300, button->y + 10, changeinprice );

		// Draw a bounding box

		if ( highlighted ) {

			glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
			border_draw ( button );

		}

	}

}

void SharesListScreenInterface::ShareMouseDown ( Button *button )
{

	UplinkAssert ( button );
	
	int shareindex;
	sscanf ( button->name, "shareslistscreen_share %d", &shareindex );
	shareindex += baseoffset;

	LList <char *> *filteredlist = &((SharesListScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ())->filteredlist;

	if ( filteredlist->ValidIndex (shareindex) ) {

		button_click ( button );

	}

}

void SharesListScreenInterface::ShareMouseMove ( Button *button )
{

	UplinkAssert ( button );
	
	int shareindex;
	sscanf ( button->name, "shareslistscreen_share %d", &shareindex );
	shareindex += baseoffset;

	LList <char *> *filteredlist = &((SharesListScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ())->filteredlist;

	if ( filteredlist->ValidIndex (shareindex) ) {
 
		button_highlight ( button );

	}

}

void SharesListScreenInterface::FilterDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

	textbutton_draw ( button, highlighted, clicked );

	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
	border_draw ( button );

}

void SharesListScreenInterface::FilterClick ( Button *button )
{

	UplinkAssert (button);

	SharesListScreenInterface *thisinterface = ((SharesListScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ());
	UplinkAssert (thisinterface);

	char *filter = EclGetButton ( "shareslistscreen_filtertext" )->caption;

	if ( filter [0] == '\x0' )  thisinterface->ApplyFilter ( NULL );
	else						thisinterface->ApplyFilter ( filter );

}

void SharesListScreenInterface::ScrollUpClick ( Button *button )
{

	--baseoffset;
	if ( baseoffset < 0 ) baseoffset = 0;

	for ( int i = 0; i < 14; ++i ) {

		char name [128];
		UplinkSnprintf ( name, sizeof ( name ), "shareslistscreen_share %d", i );
		EclDirtyButton ( name );

	}

}

void SharesListScreenInterface::ScrollDownClick ( Button *button )
{

	++baseoffset;

	for ( int i = 0; i < 14; ++i ) {

		char name [128];
		UplinkSnprintf ( name, sizeof ( name ), "shareslistscreen_share %d", i );
		EclDirtyButton ( name );

	}

}

void SharesListScreenInterface::CloseClick ( Button *button )
{

	SharesListScreenInterface *lsi = (SharesListScreenInterface *) GetInterfaceScreen ( SCREEN_SHARESLISTSCREEN );
	UplinkAssert (lsi);

	int nextpage = lsi->GetComputerScreen ()->nextpage;

	if ( nextpage != -1 ) 
		game->GetInterface ()->GetRemoteInterface ()->RunScreen ( nextpage, lsi->GetComputerScreen ()->GetComputer () );		
	
}

void SharesListScreenInterface::SetFullList ( LList <char *> *newfulllist )
{

	UplinkAssert (newfulllist);

	fulllist.Empty ();
	filteredlist.Empty ();

	for ( int i = 0; i < newfulllist->Size (); ++i )
		fulllist.PutData ( newfulllist->GetData (i) );
	
	ApplyFilter ( NULL );

}

void SharesListScreenInterface::SetFullList ()
{

	fulllist.Empty ();

	for ( int i = 0; i < filteredlist.Size (); ++i )
		fulllist.PutData ( filteredlist.GetData (i) );

	ApplyFilter ( NULL );

}

void SharesListScreenInterface::ApplyFilter ( char *filter )
{

	//
	// Do the filtering
	//

	filteredlist.Empty ();

	if ( filter ) {

		char *lowercasefilter = LowerCaseString ( filter );

		for ( int i = 0; i < fulllist.Size (); ++i ) {

			char *companyname = LowerCaseString ( fulllist.GetData (i) );

			if ( strstr ( companyname, lowercasefilter ) != NULL )
				filteredlist.PutData ( fulllist.GetData (i) );

            delete [] companyname;
				
		}

		delete [] lowercasefilter;

	}
	else {

		for ( int i = 0; i < fulllist.Size (); ++i ) 
			filteredlist.PutData ( fulllist.GetData (i) );

	}

	baseoffset = 0;

	//
	// Dirty the buttons
	//

	for ( int ib = 0; ib < 14; ++ib ) {

		char name [128];
		UplinkSnprintf ( name, sizeof ( name ), "shareslistscreen_share %d", ib );
		EclDirtyButton ( name );

	}

}

bool SharesListScreenInterface::ReturnKeyPressed ()
{

	Button *filterbutton = EclGetButton ( "shareslistscreen_filtertext" );
	
	if ( filterbutton ) {
		char *filter = filterbutton->caption;
		if ( filter [0] == '\x0' )  ApplyFilter ( NULL );
		else						ApplyFilter ( filter );
		return true;
	}

	return false;
}

void SharesListScreenInterface::Create ()
{

	if ( cs ) Create ( cs );
	else printf ( "SharesListScreenInterface::Create, tried to create when cs==NULL\n" );

}

void SharesListScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert ( newcs );
	cs = newcs;

	if ( !IsVisible () ) {

		// Draw the screen titles

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "shareslistscreen_maintitle" );
		EclRegisterButtonCallbacks ( "shareslistscreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "shareslistscreen_subtitle" );
		EclRegisterButtonCallbacks ( "shareslistscreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

		EclRegisterButton ( 30, 120, 176, 15, "Company name", "", "shareslistscreen_companytitle" );
		EclRegisterButton ( 210, 120, 96, 15, "Price per share", "", "shareslistscreen_pricetitle" );
		EclRegisterButton ( 310, 120, 70, 15, "Change (Qtr)", "", "shareslistscreen_changetitle" );
		EclRegisterButton ( 390, 120, 15, 15, "", "", "shareslistscreen_topright" );

		// Create the share entries

		for ( int li = 0; li < 14; ++li ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "shareslistscreen_share %d", li );
			EclRegisterButton ( 30, 145 + li * 15, 350, 14, "", "View this share in more detail", name );
			EclRegisterButtonCallbacks ( name, ShareDraw, ShareClick, ShareMouseDown, ShareMouseMove );

		}
		
		//
		// Set the full list depending on the screen type
		//

		if ( GetComputerScreen ()->SCREENTYPE == SHARESLISTSCREENTYPE_USERSHARES ) {

            // No longer needed

		}
		else if ( GetComputerScreen ()->SCREENTYPE == SHARESLISTSCREENTYPE_ALLSHARES ) {

			LList <char *> allshares;
			
			DArray <Company *> *allcompanies = game->GetWorld ()->companies.ConvertToDArray ();

			for ( int i = 0; i < allcompanies->Size (); ++i ) {
				if ( allcompanies->ValidIndex (i) ) {
					Company *company = allcompanies->GetData (i);
					UplinkAssert (company);
					if ( company->TYPE == COMPANYTYPE_INDUSTRIAL || 
						 company->TYPE == COMPANYTYPE_COMMERCIAL ||
						 company->TYPE == COMPANYTYPE_FINANCIAL )
						 allshares.PutData ( company->name );
				}
			}

			delete allcompanies;
					
			SetFullList ( &allshares );

		}

		if ( fulllist.Size () >= 14 ) {

			// Create the scrollbar

			EclRegisterButton ( 390, 145, 15, 15, "^", "Scroll upwards", "shareslistscreen_scrollup" );
			EclRegisterButton ( 390, 162, 15, 12 * 15 - 4, "", "", "shareslistscreen_scrollbar" );
			EclRegisterButton ( 390, 145 + 13 * 15, 15, 15, "v", "Scroll downwards", "shareslistscreen_scrolldown" );

			button_assignbitmaps ( "shareslistscreen_scrollup", "up.tif", "up_h.tif", "up_c.tif" );
			button_assignbitmaps ( "shareslistscreen_scrolldown", "down.tif", "down_h.tif", "down_c.tif" );
			EclRegisterButtonCallback ( "shareslistscreen_scrollup", ScrollUpClick );
			EclRegisterButtonCallback ( "shareslistscreen_scrolldown", ScrollDownClick );

			// Create the filter buttons

			EclRegisterButton ( 30, 365, 120, 15, "Filter", "Click here to apply the filter", "shareslistscreen_filter" );
			EclRegisterButtonCallback ( "shareslistscreen_filter", FilterClick );
			
			EclRegisterButton ( 155, 365, 200, 15, "", "Enter filter pattern here", "shareslistscreen_filtertext" );
			EclRegisterButtonCallbacks ( "shareslistscreen_filtertext", FilterDraw, NULL, button_click, button_highlight );

            EclMakeButtonEditable ( "shareslistscreen_filtertext" );

		}

		// Create the exit button

		if ( GetComputerScreen ()->nextpage != -1 ) {

			EclRegisterButton ( 360, 365, 45, 15, "Close", "Close this screen", "shareslistscreen_close" );
			EclRegisterButtonCallback ( "shareslistscreen_close", CloseClick );

		}

		ApplyFilter ( NULL );

	}

}

void SharesListScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "shareslistscreen_maintitle" );
		EclRemoveButton ( "shareslistscreen_subtitle" );

		EclRemoveButton ( "shareslistscreen_companytitle" );
		EclRemoveButton ( "shareslistscreen_pricetitle" );
		EclRemoveButton ( "shareslistscreen_changetitle" );
		EclRemoveButton ( "shareslistscreen_topright" );

		EclRemoveButton ( "shareslistscreen_filter" );
		EclRemoveButton ( "shareslistscreen_filtertext" );
		
		for ( int i = 0; i < 14; ++i ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "shareslistscreen_share %d", i );
			EclRemoveButton ( name );
		
		}

		if ( EclGetButton ( "shareslistscreen_scrollbar" ) != NULL ) {

			EclRemoveButton ( "shareslistscreen_scrollup" );
			EclRemoveButton ( "shareslistscreen_scrollbar" );
			EclRemoveButton ( "shareslistscreen_scrolldown" );

		}

		EclRemoveButton ( "shareslistscreen_close" );

	}

}

bool SharesListScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "shareslistscreen_maintitle" ) != 0 );

}

void SharesListScreenInterface::Update ()
{

}

SharesListScreen *SharesListScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);
	return (SharesListScreen *) cs;

}

int SharesListScreenInterface::ScreenID ()
{

	return SCREEN_SHARESLISTSCREEN;

}

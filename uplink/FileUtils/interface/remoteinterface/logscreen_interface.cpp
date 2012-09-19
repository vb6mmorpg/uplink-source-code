
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h>

#include "eclipse.h"
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/serialise.h"

#include "options/options.h"

#include "game/game.h"

#include "interface/interface.h"
#include "interface/scrollbox.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/logscreen_interface.h"
#include "interface/taskmanager/taskmanager.h"

#include "world/world.h"
#include "world/message.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/logbank.h"
#include "world/computer/computerscreen/logscreen.h"

#include "mmgr.h"


int LogScreenInterface::baseoffset = 0;
int LogScreenInterface::previousnumlogs = 0;


void LogScreenInterface::CloseClick ( Button *button )
{

	ComputerScreen *compscreen = game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ()->GetComputerScreen ();
	UplinkAssert( compscreen );
	UplinkAssert( compscreen->GetOBJECTID () == OID_LOGSCREEN );
	LogScreen *ls = (LogScreen *) compscreen;
    UplinkAssert (ls);
    
	if ( ls->nextpage != -1 ) 
        game->GetInterface ()->GetRemoteInterface ()->RunScreen ( ls->nextpage, ls->GetComputer () );

}

bool LogScreenInterface::EscapeKeyPressed ()
{

    CloseClick ( NULL );
    return true;
    
}

void LogScreenInterface::LogClick ( Button *button )
{

	UplinkAssert ( button );

	int logindex;
	sscanf ( button->name, "logscreen_log %d", &logindex );
	logindex = baseoffset - logindex;

	ComputerScreen *compscreen = game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ()->GetComputerScreen ();
	UplinkAssert( compscreen );
	UplinkAssert( compscreen->GetOBJECTID () == OID_LOGSCREEN );
	LogScreen *ls = (LogScreen *) compscreen;
	LogBank *logbank = ls->GetTargetLogBank ();

	Computer *comp = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetComputer ();
	UplinkAssert (comp);

	if ( comp->security.IsRunning_Proxy () ) {
		create_msgbox ( "Error", "Denied access by Proxy Server" );
		return;
	}

	// Even if the index isn't valid send the event anyway, this will permit the erasing of _blank_ spaces
	// created by stopping the log deleter in middle of it's run.

	// if ( logbank->logs.ValidIndex (logindex) )
	game->GetInterface ()->GetTaskManager ()->SetProgramTarget ( logbank, button->name, logindex );

}

/*
void LogScreenInterface::ScrollUpClick ( Button *button ) 
{

	++baseoffset;	

	LogScreen *ls = (LogScreen *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ()->GetComputerScreen ();
	int numlogs = ls->GetTargetLogBank ()->logs.Size ();
		
	if ( baseoffset >= numlogs ) baseoffset = numlogs - 1;

	for ( int i = 0; i < 15; ++i ) {

		char name [128];
		UplinkSnprintf ( name, sizeof ( name ), "logscreen_log %d", i );
		EclDirtyButton ( name );

	}

}

void LogScreenInterface::ScrollDownClick ( Button *button )
{

	--baseoffset;
	if ( baseoffset < 0 ) baseoffset = 0;

	for ( int i = 0; i < 15; ++i ) {

		char name [128];
		UplinkSnprintf ( name, sizeof ( name ), "logscreen_log %d", i );
		EclDirtyButton ( name );

	}

}
*/

int LogScreenInterface::GetNbItems ( DArray<class AccessLog *> *logs )
{

	int firstIndex = -1, lastIndex = -1;
	for ( int i = 0; i < logs->Size (); i++ )
		if ( logs->ValidIndex ( i ) ) {
			if ( firstIndex == -1 )
				firstIndex = i;

			lastIndex = i;
		}

	if ( firstIndex == -1 || lastIndex == -1 )
		return 0;
	else
		return ( lastIndex - firstIndex ) + 1;

}

int LogScreenInterface::GetFirstItem ( DArray<class AccessLog *> *logs )
{

	for ( int i = 0; i < logs->Size (); i++ )
		if ( logs->ValidIndex ( i ) )
			return i;

	return -1;

}

int LogScreenInterface::GetLastItem ( DArray<class AccessLog *> *logs )
{

	for ( int i = logs->Size () - 1; i >= 0 ; i-- )
		if ( logs->ValidIndex ( i ) )
			return i;

	return -1;

}

void LogScreenInterface::ScrollChange ( char *scrollname, int newValue )
{

	ComputerScreen *compscreen = game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ()->GetComputerScreen ();
	UplinkAssert( compscreen );
	UplinkAssert( compscreen->GetOBJECTID () == OID_LOGSCREEN );
	LogScreen *ls = (LogScreen *) compscreen;
	LogBank *logbank = ls->GetTargetLogBank ();

    baseoffset = GetLastItem ( &logbank->logs ) - newValue;

	for ( int i = 0; i < 15; ++i ) {

		char name [128];
		UplinkSnprintf ( name, sizeof ( name ), "logscreen_log %d", i );
		EclDirtyButton ( name );

	}

}

void LogScreenInterface::LogDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert ( button );

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );
	
	int logindex;
	sscanf ( button->name, "logscreen_log %d", &logindex );
	logindex = baseoffset - logindex;

	ComputerScreen *compscreen = game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ()->GetComputerScreen ();
	UplinkAssert( compscreen );
	UplinkAssert( compscreen->GetOBJECTID () == OID_LOGSCREEN );
	LogScreen *ls = (LogScreen *) compscreen;
	LogBank *logbank = ls->GetTargetLogBank ();

	if ( logbank->logs.ValidIndex (logindex) ) {

		AccessLog *log = logbank->logs.GetData (logindex);

		if ( logindex % 2 == 0 ) {

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

		glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
		GciDrawText ( button->x + 10, button->y + 10, log->date.GetShortString () );
		
		char *description = log->GetDescription ();
		GciDrawText ( button->x + 100, button->y + 10, description );
		delete [] description;

		// Draw a bounding box
		if ( highlighted ) {

			glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
			border_draw ( button );

		}

	}
	else {

		clear_draw ( button->x, button->y, button->width, button->height );

	}
	
	glDisable ( GL_SCISSOR_TEST );

}

void LogScreenInterface::Create ()
{

	if ( cs ) Create ( cs );
	else printf ( "LogScreenInterface::Create, tried to create when LogScreen==NULL\n" );

}

void LogScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert ( newcs );
	cs = newcs;

	if ( !IsVisible () ) {

		LogBank *logbank = GetComputerScreen ()->GetTargetLogBank ();		
		UplinkAssert (logbank);

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "logscreen_maintitle" );
		EclRegisterButtonCallbacks ( "logscreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "logscreen_subtitle" );
		EclRegisterButtonCallbacks ( "logscreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

		EclRegisterButton ( 15, 120, 96, 15, "Date", "", "logscreen_datetitle" );
		EclRegisterButton ( 115, 120, 300, 15, "Action", "", "logscreen_actiontitle" );
		
		// Create the log entries

		for ( int i = 0; i < 15; ++i ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "logscreen_log %d", i );
			EclRegisterButton ( 15, 140 + i * 15, 400, 14, "", "Select this log", name );
			EclRegisterButtonCallbacks ( name, LogDraw, LogClick, button_click, button_highlight );

		}

		/*
		if ( logbank->logs.Size () >= 15 ) {
		*/
		int nbLogs = GetNbItems ( &logbank->logs );

		if ( nbLogs >= 15 ) {

			// Create the scroll bar

			CreateScrollBar ( nbLogs );

			/*
			EclRegisterButton ( 420, 140, 15, 15, "^", "Scroll up", "logscreen_scrollup" );
			button_assignbitmaps ( "logscreen_scrollup", "up.tif", "up_h.tif", "up_c.tif" );
			EclRegisterButtonCallback ( "logscreen_scrollup", ScrollUpClick );

			EclRegisterButton ( 420, 157, 15, 13 * 15 - 4, "", "logscreen_scrollbar" );

			EclRegisterButton ( 420, 140 + 14 * 15, 15, 15, "v", "Scroll down", "logscreen_scrolldown" );
			button_assignbitmaps ( "logscreen_scrolldown", "down.tif", "down_h.tif", "down_c.tif" );
			EclRegisterButtonCallback ( "logscreen_scrolldown", ScrollDownClick );
			*/

		}

		// Create the close button

		EclRegisterButton ( 421, 121, 13, 13, "", "Close the Log Screen", "logscreen_close" );
		button_assignbitmaps ( "logscreen_close", "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( "logscreen_close", CloseClick );

		// Set the baseoffset to be the last entry in the logs (newest)

		/*
		baseoffset = logbank->logs.Size () - 1;
		*/
		baseoffset = GetLastItem ( &logbank->logs );

	}

}

void LogScreenInterface::CreateScrollBar ( int nbItems )
{

    ScrollBox::CreateScrollBox( "logscreen_scroll", 420, 140, 15, 
	                            15 * 15, nbItems, 15, 0,  ScrollChange );

}

void LogScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "logscreen_maintitle" );
		EclRemoveButton ( "logscreen_subtitle" );

		EclRemoveButton ( "logscreen_datetitle" );
		EclRemoveButton ( "logscreen_actiontitle" );

		for ( int i = 0; i < 15; ++i ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "logscreen_log %d", i );
			EclRemoveButton ( name );

		}

		/*
		EclRemoveButton ( "logscreen_scrollup" );
		EclRemoveButton ( "logscreen_scrollbar" );
		EclRemoveButton ( "logscreen_scrolldown" );
		*/
        if ( ScrollBox::GetScrollBox( "logscreen_scroll" ) != NULL )
            ScrollBox::RemoveScrollBox( "logscreen_scroll" );

        EclRemoveButton ( "logscreen_close" );

	}

}

bool LogScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "logscreen_maintitle" ) != NULL );

}

void LogScreenInterface::Update ()
{

	ComputerScreen *compscreen = game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ()->GetComputerScreen ();
	UplinkAssert( compscreen );
	UplinkAssert( compscreen->GetOBJECTID () == OID_LOGSCREEN );
	LogScreen *ls = (LogScreen *) compscreen;
	LogBank *logbank = ls->GetTargetLogBank ();

	/*
	int newnumlogs = logbank->logs.NumUsed ();
	*/
	int newnumlogs = GetNbItems ( &logbank->logs );

	if ( newnumlogs != previousnumlogs ) {

		for ( int i = 0; i < 15; ++i ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "logscreen_log %d", i );
			EclDirtyButton ( name );

		}

		previousnumlogs = newnumlogs;

		//
		// Update the scrollbar
		//

		/*
		ScrollBox *sb = ScrollBox::GetScrollBox("logscreen_scroll");
		if ( sb )
			sb->SetNumItems( newnumlogs );
		*/

		int indexLastItem = GetLastItem ( &logbank->logs );
		ScrollBox *sb = ScrollBox::GetScrollBox("logscreen_scroll");

		if ( newnumlogs >= 15 ) {
			if ( !sb ) {
				baseoffset = indexLastItem;
				CreateScrollBar ( newnumlogs );
			}
			else {
				if ( indexLastItem < baseoffset )
					baseoffset = indexLastItem;
				sb->SetNumItems( newnumlogs );
			}
		}
		else {
			baseoffset = indexLastItem;
			if ( sb )
				ScrollBox::RemoveScrollBox( "logscreen_scroll" );
		}



	}

}

LogScreen *LogScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);
	return (LogScreen *) cs;

}

int LogScreenInterface::ScreenID ()
{

	return SCREEN_LOGSCREEN;

}

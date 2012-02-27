// DialogScreenInterface.cpp: implementation of the DialogScreenInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif

#ifndef HAVE_GLES
#include <GL/gl.h>
#include <GL/glu.h> /* glu extention library */
#else
#include <GLES/gl.h>
#include <GLES/glues.h>
#endif

#include "eclipse.h"

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "game/game.h"
#include "game/scriptlibrary.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/dialogscreen_interface.h"
#include "interface/localinterface/keyboardinterface.h"

#include "world/world.h"
#include "world/computer/computer.h"
#include "world/computer/recordbank.h"
#include "world/computer/computerscreen/dialogscreen.h"

#include "mmgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void DialogScreenInterface::PasswordBoxDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

	clear_draw ( button->x, button->y, button->width, button->height );

	glColor4f ( 1.0f, 1.0f, 1.0f, ALPHA );    

	// Print the text

	char *caption = new char [strlen(button->caption) + 1];
	for ( unsigned i = 0; i < strlen(button->caption); ++i )
		caption [i] = '*';

	caption [strlen(button->caption)] = '\x0';
	GciDrawText ( button->x + 10, button->y + 10, caption, BITMAP_15 );

	delete [] caption;

	// Draw a box around the text if highlighted

	if ( highlighted || clicked )
		border_draw ( button );

    glDisable ( GL_SCISSOR_TEST );

}

void DialogScreenInterface::NextPageClick ( Button *button )
{

	UplinkAssert ( button );

	char bname [64 + SIZE_VLOCATION_IP + 1];
	int data1, data2;
	char ip [ SIZE_VLOCATION_IP ] = {0};
	sscanf ( button->name, "%s %d %d %s", bname, &data1, &data2, ip );

	int nextpage = data1;

	Computer *comp = NULL;
	if ( ip ) {
		VLocation *loc = game->GetWorld ()->GetVLocation ( ip );
		if ( loc )
			comp = loc->GetComputer ();
	}
	if ( nextpage != -1 ) game->GetInterface ()->GetRemoteInterface ()->RunScreen ( nextpage, comp );

}

void DialogScreenInterface::ScriptButtonClick ( Button *button )
{

	UplinkAssert ( button );

	char bname [64 + SIZE_VLOCATION_IP + 1];
	int data1, data2;
	char ip [ SIZE_VLOCATION_IP ] = {0};
	sscanf ( button->name, "%s %d %d %s", bname, &data1, &data2, ip );

	int scriptindex = data1;
	int nextpage = data2;

	if ( scriptindex != -1 ) ScriptLibrary::RunScript ( scriptindex );

	Computer *comp = NULL;
	if ( ip ) {
		VLocation *loc = game->GetWorld ()->GetVLocation ( ip );
		if ( loc )
			comp = loc->GetComputer ();
	}
	if ( nextpage != -1 ) game->GetInterface ()->GetRemoteInterface ()->RunScreen ( nextpage, comp );

}

bool DialogScreenInterface::ReturnKeyPressed ()
{

	DialogScreen *ds = GetComputerScreen ();
	UplinkAssert (ds);
	char *returnkeybutton = ds->returnkeybutton;

	if ( returnkeybutton ) {

		for ( int i = 0; i < ds->widgets.Size (); ++i ) {

			DialogScreenWidget *dsw = ds->widgets.GetData (i);
			UplinkAssert (dsw);

			if ( strcmp ( dsw->GetName (), returnkeybutton ) == 0 ) {

				char name [64 + SIZE_VLOCATION_IP + 1];
				UplinkSnprintf ( name, sizeof ( name ), "%s %d %d", dsw->GetName (), dsw->data1, dsw->data2 );		
				Button *button = EclGetButton (name);
				if ( !button ) {
					UplinkSnprintf ( name, sizeof ( name ), "%s %d %d %s", dsw->GetName (), dsw->data1, dsw->data2, ds->GetComputer ()->ip );		
					button = EclGetButton (name);
				}
				if ( button ) button->MouseUp ();
				return true;

			}

		}

	}

	return false;

}

bool DialogScreenInterface::EscapeKeyPressed ()
{

	DialogScreen *ds = GetComputerScreen ();
	UplinkAssert (ds);
	char *escapekeybutton = ds->escapekeybutton;

	if ( escapekeybutton ) {

		for ( int i = 0; i < ds->widgets.Size (); ++i ) {

			DialogScreenWidget *dsw = ds->widgets.GetData (i);
			UplinkAssert (dsw);

			if ( strcmp ( dsw->GetName (), escapekeybutton ) == 0 ) {

				char name [64 + SIZE_VLOCATION_IP + 1];
				UplinkSnprintf ( name, sizeof ( name ), "%s %d %d", dsw->GetName (), dsw->data1, dsw->data2 );		
				Button *button = EclGetButton (name);
				if ( !button ) {
					UplinkSnprintf ( name, sizeof ( name ), "%s %d %d %s", dsw->GetName (), dsw->data1, dsw->data2, ds->GetComputer ()->ip );		
					button = EclGetButton (name);
				}
				if ( button ) button->MouseUp ();
				return true;

			}

		}

	}

	return false;

}

DialogScreenInterface::DialogScreenInterface()
{

}

DialogScreenInterface::~DialogScreenInterface()
{

}

void DialogScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert (newcs);
	cs = newcs;

	if ( !IsVisible () ) {

		EclRegisterButton ( 0, 0, 0, 0, " ", " ", "dialogscreen" );
		EclRegisterButtonCallbacks ( "dialogscreen", NULL, NULL, NULL, NULL );

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "dialogscreen_maintitle" );
		EclRegisterButtonCallbacks ( "dialogscreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "dialogscreen_subtitle" );
		EclRegisterButtonCallbacks ( "dialogscreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

		DialogScreen *ds = GetComputerScreen ();

		// Draw each widget in this dialogscreen

		Computer *comp = ds->GetComputer ();

		for ( int i = 0; i < ds->widgets.Size (); ++i ) {

			DialogScreenWidget *dsw = ds->widgets.GetData (i);
			UplinkAssert (dsw);

			char name [64];
			UplinkSnprintf ( name, sizeof ( name ), "%s %d %d", dsw->GetName (), dsw->data1, dsw->data2 );

			char nameip [64 + SIZE_VLOCATION_IP + 1];
			UplinkSnprintf ( nameip, sizeof ( nameip ), "%s %d %d %s", dsw->GetName (), dsw->data1, dsw->data2, comp->ip );

			// Customize the widget depending on its type

			switch ( dsw->WIDGETTYPE ) {

				case WIDGET_BASIC		:		EclRegisterButton ( dsw->x, dsw->y, dsw->width, dsw->height, dsw->GetCaption (), dsw->GetTooltip (), name );
												EclRegisterButtonCallbacks ( name, button_draw, NULL, NULL, NULL );
												break;

				case WIDGET_CAPTION		:		EclRegisterButton ( dsw->x, dsw->y, dsw->width, dsw->height, dsw->GetCaption (), dsw->GetTooltip (), name );
												EclRegisterButtonCallbacks ( name, textbutton_draw, NULL, NULL, NULL );
												break;

				case WIDGET_TEXTBOX		:		EclRegisterButton ( dsw->x, dsw->y, dsw->width, dsw->height, dsw->GetCaption (), dsw->GetTooltip (), name );
												EclRegisterButtonCallbacks ( name, textbutton_draw, NULL, button_click, button_highlight );		
												EclMakeButtonEditable ( name );
												break;

				case WIDGET_PASSWORDBOX :       EclRegisterButton ( dsw->x, dsw->y, dsw->width, dsw->height, dsw->GetCaption (), dsw->GetTooltip (), name );
												EclRegisterButtonCallbacks ( name, PasswordBoxDraw, NULL, button_click, button_highlight );
												EclMakeButtonEditable ( name );
												break;

				case WIDGET_NEXTPAGE    :		EclRegisterButton ( dsw->x, dsw->y, dsw->width, dsw->height, dsw->GetCaption (), dsw->GetTooltip (), nameip );
												EclRegisterButtonCallbacks ( nameip, button_draw, NextPageClick, button_click, button_highlight );
												break; 
				
				case WIDGET_IMAGE		:       EclRegisterButton ( dsw->x, dsw->y, dsw->width, dsw->height, dsw->GetCaption (), dsw->GetTooltip (), name );
												button_assignbitmaps ( name, dsw->GetStringData1 (), dsw->GetStringData2 (), dsw->GetStringData2 () );
												break;

				case WIDGET_IMAGEBUTTON :       EclRegisterButton ( dsw->x, dsw->y, dsw->width, dsw->height, dsw->GetCaption (), dsw->GetTooltip (), nameip );
												button_assignbitmaps ( nameip, dsw->GetStringData1 (), dsw->GetStringData2 (), dsw->GetStringData2 () );
												EclRegisterButtonCallback ( nameip, NextPageClick );
												break;

				case WIDGET_SCRIPTBUTTON:       EclRegisterButton ( dsw->x, dsw->y, dsw->width, dsw->height, dsw->GetCaption (), dsw->GetTooltip (), nameip );
												EclRegisterButtonCallbacks ( nameip, button_draw, ScriptButtonClick, button_click, button_highlight );
												break;
	
				case WIDGET_FIELDVALUE  :		EclRegisterButton ( dsw->x, dsw->y, dsw->width, dsw->height, dsw->GetCaption (), dsw->GetTooltip (), name );
												{
													EclRegisterButtonCallbacks ( name, textbutton_draw, NULL, NULL, NULL );
													EclMakeButtonEditable ( name );
													Record *rec = ds->GetComputer ()->recordbank.GetRecordFromName ( game->GetInterface ()->GetRemoteInterface ()->security_name );
													if ( rec ) {
														char *value = rec->GetField ( dsw->GetStringData1 () );
														if ( value ) EclGetButton ( name )->SetCaption ( value );
													}
												}
												break;

				case WIDGET_NONE		:		printf ( "Error : DialogScreenInterface::Create, WIDGETTYPE==NONE\n" );
												break;

			}

		}

	}

}

void DialogScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		DialogScreen *ds = GetComputerScreen ();
		UplinkAssert (ds);

		EclRemoveButton ( "dialogscreen" );

		EclRemoveButton ( "dialogscreen_maintitle" );
		EclRemoveButton ( "dialogscreen_subtitle" );

		Computer *comp = ds->GetComputer ();

		for ( int i = 0; i < ds->widgets.Size (); ++i ) {

			DialogScreenWidget *dsw = ds->widgets.GetData (i);

			RemoveWidget ( dsw, comp );

		}

	}

}

void DialogScreenInterface::RemoveWidget ( DialogScreenWidget *dsw, Computer *comp )
{

	UplinkAssert (dsw);
	UplinkAssert (comp);

	char name [64];
	UplinkSnprintf ( name, sizeof ( name ), "%s %d %d", dsw->GetName (), dsw->data1, dsw->data2 );

	EclRemoveButton ( name );

	char nameip [64 + SIZE_VLOCATION_IP + 1];
	UplinkSnprintf ( nameip, sizeof ( nameip ), "%s %d %d %s", dsw->GetName (), dsw->data1, dsw->data2, comp->ip );

	EclRemoveButton ( nameip );

}

bool DialogScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "dialogscreen" ) != NULL );

}

int DialogScreenInterface::ScreenID ()
{

	return SCREEN_DIALOGSCREEN;

}

DialogScreen *DialogScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);
	return (DialogScreen *) cs;

}


// DialogScreen.cpp: implementation of the DialogScreen class.
//
//////////////////////////////////////////////////////////////////////

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/dialogscreen_interface.h"

#include "world/computer/computerscreen/dialogscreen.h"

#include "mmgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DialogScreen::DialogScreen()
{

	returnkeybutton = NULL;
    escapekeybutton = NULL;

}

DialogScreen::~DialogScreen()
{
	
	DeleteLListData ( (LList <UplinkObject *> *) &widgets );

    if ( returnkeybutton ) delete [] returnkeybutton;
    if ( escapekeybutton ) delete [] escapekeybutton;

}

void DialogScreen::AddWidget ( char *name, int WIDGETTYPE, int x, int y, int width, int height, char *caption, char *tooltip )
{

	DialogScreenWidget *dsw = new DialogScreenWidget ();
	
	dsw->SetName ( name );
	dsw->SetTYPE ( WIDGETTYPE );
	dsw->SetPosition ( x, y );
	dsw->SetSize ( width, height );
	dsw->SetCaption ( caption );
	dsw->SetTooltip ( tooltip );

	widgets.PutData ( dsw );

}

void DialogScreen::AddWidget ( char *name, int WIDGETTYPE, int x, int y, int width, int height, char *caption, char *tooltip, 
							   int data1, int data2, char *stringdata1, char *stringdata2 )
{

	DialogScreenWidget *dsw = new DialogScreenWidget ();
	
	dsw->SetName ( name );
	dsw->SetTYPE ( WIDGETTYPE );
	dsw->SetPosition ( x, y );
	dsw->SetSize ( width, height );
	dsw->SetCaption ( caption );
	dsw->SetTooltip ( tooltip );

	dsw->SetData ( data1, data2 );
	dsw->SetStringData ( stringdata1, stringdata2 );

	widgets.PutData ( dsw );

}

void DialogScreen::RemoveWidget ( char *name )
{

	for ( int i = 0; i < widgets.Size (); ++i ) {
		
		DialogScreenWidget *dsw = widgets.GetData (i);
		UplinkAssert (dsw);

		if ( strcmp ( dsw->GetName (), name ) == 0 ) {

			ComputerScreen *cs = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();

			if ( cs == this )
				DialogScreenInterface::RemoveWidget ( dsw, GetComputer () );

			widgets.RemoveData (i);
            delete dsw;
			return;
		}

	}

}

void DialogScreen::SetReturnKeyButton ( char *name )
{

	if ( returnkeybutton ) delete [] returnkeybutton;
	returnkeybutton = new char [strlen(name)+1];
	UplinkSafeStrcpy ( returnkeybutton, name );
	
}

void DialogScreen::SetEscapeKeyButton ( char *name )
{

    if ( escapekeybutton ) delete [] escapekeybutton;
    escapekeybutton = new char [strlen(name)+1];
    UplinkSafeStrcpy ( escapekeybutton, name );

}

bool DialogScreen::Load ( FILE *file )
{
	
	LoadID ( file );

	if ( !ComputerScreen::Load ( file ) ) return false;

	if ( !LoadLList ( (LList <UplinkObject *> *) &widgets, file ) ) return false;

	if ( !LoadDynamicStringPtr ( &returnkeybutton, file ) ) return false;
    if ( !LoadDynamicStringPtr ( &escapekeybutton, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void DialogScreen::Save ( FILE *file )
{

	SaveID ( file );

	ComputerScreen::Save ( file );

	SaveLList ( (LList <UplinkObject *> *) &widgets, file );

	SaveDynamicString ( returnkeybutton, file );
    SaveDynamicString ( escapekeybutton, file );

	SaveID_END ( file );

}

void DialogScreen::Print ()
{

	printf ( "DialogScreen\n" );
	ComputerScreen::Print ();
	PrintLList ( (LList <UplinkObject *> *) &widgets );
	printf ( "ReturnKeyButton = %s\n", returnkeybutton );
    printf ( "EscapeKeyButton = %s\n", escapekeybutton );

}
	
char *DialogScreen::GetID ()
{
	
	return "SCR_DLG";

}

int DialogScreen::GetOBJECTID ()
{

	return OID_DIALOGSCREEN;

}			


// ============================================================================
//  Dialog Widget


DialogScreenWidget::DialogScreenWidget ()
{

	x = y = width = height = 0;
	name = caption = tooltip = NULL;
	WIDGETTYPE = WIDGET_NONE;

	data1 = data2 = 0;
	stringdata1 = stringdata2 = NULL;

}

DialogScreenWidget::~DialogScreenWidget ()
{

	if ( name )	   delete [] name;
	if ( caption ) delete [] caption;
	if ( tooltip ) delete [] tooltip;

	if ( stringdata1 ) delete [] stringdata1;
	if ( stringdata2 ) delete [] stringdata2;

}

void DialogScreenWidget::SetName ( char *newname )
{

	if ( name ) delete [] name;
	name = new char [strlen(newname)+1];
	UplinkSafeStrcpy ( name, newname );

}

void DialogScreenWidget::SetTYPE ( int NEWWIDGETTYPE )
{

	WIDGETTYPE = NEWWIDGETTYPE;

}

void DialogScreenWidget::SetPosition ( int newx, int newy )
{
	
	x = newx;
	y = newy;

}

void DialogScreenWidget::SetSize ( int newwidth, int newheight )
{

	width = newwidth;
	height = newheight;

}

void DialogScreenWidget::SetTooltip ( char *newtooltip )
{

	if ( tooltip ) delete [] tooltip;
	tooltip = new char [strlen(newtooltip) + 1];
	UplinkSafeStrcpy ( tooltip, newtooltip );

}

void DialogScreenWidget::SetCaption ( char *newcaption )
{

	if ( caption ) delete [] caption;
	caption = new char [strlen(newcaption) + 1];
	UplinkSafeStrcpy ( caption, newcaption );

}
	
char *DialogScreenWidget::GetName ()
{

	return name;

}

char *DialogScreenWidget::GetTooltip ()
{

	return tooltip;

}

char *DialogScreenWidget::GetCaption ()
{

	return caption;

}


void DialogScreenWidget::SetData ( int newdata1, int newdata2 )
{

	data1 = newdata1;
	data2 = newdata2;

}

void DialogScreenWidget::SetStringData ( char *newstringdata1, char *newstringdata2 )
{

	if ( stringdata1 ) delete [] stringdata1;
	stringdata1 = NULL;
	
	if ( newstringdata1 ) {
		stringdata1 = new char [strlen(newstringdata1) + 1];
		UplinkSafeStrcpy ( stringdata1, newstringdata1 );
	}
	
	if ( stringdata2 ) delete [] stringdata2;
	stringdata2 = NULL;

	if ( newstringdata2 ) {
		stringdata2 = new char [strlen(newstringdata2) + 1];
		UplinkSafeStrcpy ( stringdata2, newstringdata2 );
	}

}

char *DialogScreenWidget::GetStringData1 ()
{

	return stringdata1;

}

char *DialogScreenWidget::GetStringData2 ()
{

	return stringdata2;

}

bool DialogScreenWidget::Load ( FILE *file )
{
	
	LoadID ( file );

	if ( !LoadDynamicStringPtr ( &name, file ) ) return false;

	if ( !FileReadData ( &x, sizeof(x), 1, file ) ) return false;
	if ( !FileReadData ( &y, sizeof(y), 1, file ) ) return false;
	if ( !FileReadData ( &width, sizeof(width), 1, file ) ) return false;
	if ( !FileReadData ( &height, sizeof(height), 1, file ) ) return false;
	if ( !FileReadData ( &WIDGETTYPE, sizeof(WIDGETTYPE), 1, file ) ) return false;

	if ( !LoadDynamicStringPtr ( &caption, file ) ) return false;
	if ( !LoadDynamicStringPtr ( &tooltip, file ) ) return false;

	if ( !FileReadData ( &data1, sizeof(data1), 1, file ) ) return false;
	if ( !FileReadData ( &data2, sizeof(data2), 1, file ) ) return false;

	if ( !LoadDynamicStringPtr ( &stringdata1, file ) ) return false;
	if ( !LoadDynamicStringPtr ( &stringdata2, file ) ) return false;
	
	LoadID_END ( file );

	return true;

}

void DialogScreenWidget::Save ( FILE *file )
{

	SaveID ( file );

	SaveDynamicString ( name, file );

	fwrite ( &x, sizeof(x), 1, file );
	fwrite ( &y, sizeof(y), 1, file );
	fwrite ( &width, sizeof(width), 1, file );
	fwrite ( &height, sizeof(height), 1, file );
	fwrite ( &WIDGETTYPE, sizeof(WIDGETTYPE), 1, file );

	SaveDynamicString ( caption, file );
	SaveDynamicString ( tooltip, file );

	fwrite ( &data1, sizeof(data1), 1, file );
	fwrite ( &data2, sizeof(data2), 1, file );

	SaveDynamicString ( stringdata1, file );
	SaveDynamicString ( stringdata2, file );
		
	SaveID_END ( file );

}

void DialogScreenWidget::Print ()
{

	printf ( "DialogScreenWidget : Name:%s\n", name );
	printf ( "\tTYPE:%d, x:%d, y:%d, width:%d, height:%d\n", WIDGETTYPE, x, y, width, height );
	printf ( "\tcaption:%s, tooltip:%s\n", caption, tooltip );
	printf ( "\tdata1:%d, data2:%d, stringdata1:%s, stringdata2:%s\n", data1, data2, stringdata1, stringdata2 );

}
	
char *DialogScreenWidget::GetID ()
{

	return "DLG_WIDG";

}

int DialogScreenWidget::GetOBJECTID ()
{

	return OID_DIALOGSCREENWIDGET;

}
			

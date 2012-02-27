
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

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "world/world.h"
#include "world/player.h"
#include "world/generator/numbergenerator.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/sharesviewscreen_interface.h"

#include "mmgr.h"



SharesViewScreenInterface::SharesViewScreenInterface ()
{

	UplinkStrncpy ( companyname, " ", sizeof ( companyname ) );

	for ( int i = 0; i < 12; ++i )
		sharehistory [i] = 0;

	lastmonthset = 0;

}

SharesViewScreenInterface::~SharesViewScreenInterface ()
{
}

bool SharesViewScreenInterface::EscapeKeyPressed ()
{

	char closename [32];
	UplinkSnprintf ( closename, sizeof ( closename ), "sharesviewscreen_close %d", GetComputerScreen ()->nextpage );
    Button *button = EclGetButton (closename);
    UplinkAssert (closename);

    CloseClick ( button );
    return true;

}

void SharesViewScreenInterface::DrawPriceGraph ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert ( button );

	clear_draw ( button->x, button->y, button->width, button->height );

	SharesViewScreenInterface *thisint = (SharesViewScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

	if ( thisint->companyname [0] != ' ' ) {

		// Draw the axis

		glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );

#ifndef HAVE_GLES
		glBegin ( GL_LINES );
			
			glVertex2d ( button->x + 25, button->y + button->height - 40 );						// vertical
			glVertex2d ( button->x + 25, button->y + button->height - 190 );					// price

			glVertex2d ( button->x + 25, button->y + button->height - 40 );						// Horizontal
			glVertex2d ( button->x + 190, button->y + button->height - 40 );					// time
				
		glEnd ();
#else
		GLfloat verts[] = {
			button->x + 25, button->y + button->height - 40,
			button->x + 25, button->y + button->height - 190,
			button->x + 25, button->y + button->height - 40,
			button->x + 190, button->y + button->height - 40
		};

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		glVertexPointer(2, GL_FLOAT, 0, verts);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);
#endif

		GciDrawText ( 37, button->y + button->height - 40, "0" );
		GciDrawText ( 32, button->y + button->height - 190, "150" );
		GciDrawText ( 40, button->y + button->height - 30, "Time" );

		// Start on the right (now) and work left (the past)

		int monthnow = thisint->lastmonthset;
		int yearnow = game->GetWorld ()->date.GetYear ();

		glColor4f ( 0.2f, 0.2f, 1.0f, 1.0f );
		glLineWidth ( 2 );

#ifndef HAVE_GLES
		glBegin ( GL_LINE_STRIP );

		for ( int it = 0; it < 12; ++it ) {

			int month = monthnow - it;
			if ( month < 0 ) month += 12;

			char date [32];
			UplinkSnprintf ( date, sizeof ( date ), "%s, %d", Date::GetMonthName ( month + 1 ), month > monthnow ? yearnow - 1 : yearnow );

			int value = thisint->sharehistory [month];			
			glVertex2d ( button->x + 190 - ( it * 15 ), button->y + button->height - 40 - value );
					
		}

		glEnd ();
#else
		// TOOD: impelment gles
#endif
		glLineWidth ( 1 );

	}

	border_draw ( button );

}

void SharesViewScreenInterface::DrawProfit ( Button *button, bool highlighted, bool clicked )
{

    UplinkAssert (button);
    clear_draw ( button->x, button->y, button->width, button->height );
    DrawMainTitle ( button, highlighted, clicked );

}

void SharesViewScreenInterface::BuyClick ( Button *button )
{

	SharesViewScreenInterface *thisint = (SharesViewScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

	if ( thisint->companyname [0] != ' ' ) {

		game->GetWorld ()->GetPlayer ()->TradeShares ( thisint->companyname, 10 );
		
		UpdateStatus ();

	}

}

void SharesViewScreenInterface::SellClick ( Button *button )
{

	SharesViewScreenInterface *thisint = (SharesViewScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

	if ( thisint->companyname [0] != ' ' ) {

		game->GetWorld ()->GetPlayer ()->TradeShares ( thisint->companyname, -10 );
		
		UpdateStatus ();

	}

}

void SharesViewScreenInterface::CloseClick ( Button *button )
{

    GenericScreen *gs = (GenericScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
    UplinkAssert (gs);

	if ( gs->nextpage != -1 ) 
        game->GetInterface ()->GetRemoteInterface ()->RunScreen ( gs->nextpage, gs->GetComputer () );

}

void SharesViewScreenInterface::UpdateStatus ()
{

	SharesViewScreenInterface *thisint = (SharesViewScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

	if ( thisint->companyname [0] != ' ' ) {

		Company *company = game->GetWorld ()->GetCompany ( thisint->companyname );
		
		int numowned = game->GetWorld ()->GetPlayer ()->NumSharesOwned ( thisint->companyname );
		int pricepaid = game->GetWorld ()->GetPlayer ()->SharesPricePaid ( thisint->companyname );
		int currentprice = company->GetSharePrice ();
		int profit = (numowned * currentprice) - pricepaid;

        if ( game->GetInterface ()->GetRemoteInterface ()->security_level < 10 ) {
        
		    char caption [128];
		    UplinkSnprintf ( caption, sizeof ( caption ), "You own %d shares\nPaid:%dc Value:%dc", numowned, pricepaid, numowned * currentprice );
		    EclGetButton ( "sharesviewscreen_owned" )->SetCaption ( caption );

		    char profits [64];
		    UplinkSnprintf ( profits, sizeof ( profits ), "Profit:%dc", profit );
		    EclGetButton ( "sharesviewscreen_profit" )->SetCaption ( profits );

        }

	}

}

void SharesViewScreenInterface::SetCompanyName ( char *newcompanyname )
{

	UplinkAssert ( strlen(newcompanyname) < SIZE_COMPANY_NAME );
	UplinkStrncpy ( companyname, newcompanyname, sizeof ( companyname ) );

	//
	// Look up the company
	// and copy the share prices
	//

	Company *company = game->GetWorld ()->GetCompany ( companyname );
	UplinkAssert (company);

	for ( int i = 0; i < 12; ++i )
		sharehistory [i] = company->GetSharePrice ( i );

	lastmonthset = company->GetShareLastMonthSet ();

	EclGetButton ( "sharesviewscreen_subtitle" )->SetCaption ( companyname );

	//
	// Put those share prices into the text boxes
	//

	int monthnow = lastmonthset;
	int yearnow = game->GetWorld ()->date.GetYear ();

	for ( int it = 0; it < 12; ++it ) {

		int month = monthnow - it;
		if ( month < 0 ) month += 12;

		char bdate [32];
		char bname [32];
		UplinkSnprintf ( bdate, sizeof ( bdate ), "sharesviewscreen_date %d", it );
		UplinkSnprintf ( bname, sizeof ( bname ), "sharesviewscreen_numbers %d", it );

		char date [32];
		UplinkSnprintf ( date, sizeof ( date ), "%s, %d", Date::GetMonthName ( month + 1 ), month > monthnow ? yearnow - 1 : yearnow );
		char value [16];
		UplinkSnprintf ( value, sizeof ( value ), "%d c", sharehistory [month] );
		
		EclGetButton ( bdate )->SetCaption ( date );
		EclGetButton ( bname )->SetCaption ( value );

	}

	UpdateStatus ();

	//
	// Force a redraw of the graph
	//

	EclDirtyButton ( "sharesviewscreen_pricegraph" );

}

void SharesViewScreenInterface::Create ()
{

	if ( cs ) Create ( cs );
	else printf ( "SharesViewScreenInterface::Create, tried to create when cs==NULL\n" );

}

void SharesViewScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert ( newcs );
	cs = newcs;

	if ( !IsVisible () ) {

		// Draw the screen titles

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "sharesviewscreen_maintitle" );
		EclRegisterButtonCallbacks ( "sharesviewscreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "sharesviewscreen_subtitle" );
		EclRegisterButtonCallbacks ( "sharesviewscreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

		// Share value graph

		EclRegisterButton ( 30, 120, 200, 15, "Share value graph", "", "sharesviewscreen_valuetitle" );

		EclRegisterButton ( 30, 135, 200, 200, "", "", "sharesviewscreen_pricegraph" );
		EclRegisterButtonCallbacks ( "sharesviewscreen_pricegraph", DrawPriceGraph, NULL, NULL, NULL );

		// Share values (numeric)

		EclRegisterButton ( 250, 120, 150, 15, "Share values", "", "sharesviewscreen_numberstitle" );

		for ( int i = 0; i < 12; ++i ) {

			char date [32];
			char name [32];
			UplinkSnprintf ( date, sizeof ( date ), "sharesviewscreen_date %d", i );
			UplinkSnprintf ( name, sizeof ( name ), "sharesviewscreen_numbers %d", i );
			EclRegisterButton ( 250, 135 + i * 17, 100, 15, "date", "", date );
			EclRegisterButton ( 350, 135 + i * 17, 50, 15, "bla", "", name );
			EclRegisterButtonCallbacks ( date, textbutton_draw, NULL, NULL, NULL );
			EclRegisterButtonCallbacks ( name, textbutton_draw, NULL, NULL, NULL );

		}

		// Control buttons

        if ( game->GetInterface ()->GetRemoteInterface ()->security_level < 10 ) {

		    EclRegisterButton ( 30, 350, 30, 15, "Buy", "Click to buy shares in this company", "sharesviewscreen_buy" );
		    EclRegisterButtonCallback ( "sharesviewscreen_buy", BuyClick );

		    EclRegisterButton ( 30, 365, 30, 15, "Sell", "Click to sell shares in this company", "sharesviewscreen_sell" );
		    EclRegisterButtonCallback ( "sharesviewscreen_sell", SellClick );

		    EclRegisterButton ( 70, 350, 150, 30, "", "", "sharesviewscreen_owned" );
		    EclRegisterButtonCallbacks ( "sharesviewscreen_owned", textbutton_draw, NULL, NULL, NULL );

		    EclRegisterButton ( 220, 350, 140, 30, "", "", "sharesviewscreen_profit" );
		    EclRegisterButtonCallbacks ( "sharesviewscreen_profit", DrawProfit, NULL, NULL, NULL );

        }

		UpdateStatus ();

		// Close button

		EclRegisterButton ( 360, 365, 45, 15, "Close", "Return to the share listing", "sharesviewscreen_close" );
		EclRegisterButtonCallback ( "sharesviewscreen_close", CloseClick );

	}

}

void SharesViewScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "sharesviewscreen_maintitle" );
		EclRemoveButton ( "sharesviewscreen_subtitle" );

		EclRemoveButton ( "sharesviewscreen_valuetitle" );
		EclRemoveButton ( "sharesviewscreen_pricegraph" );
		EclRemoveButton ( "sharesviewscreen_numberstitle" );

		for ( int i = 0; i < 12; ++i ) {

			char date [32];
			char name [32];
			UplinkSnprintf ( date, sizeof ( date ), "sharesviewscreen_date %d", i );
			UplinkSnprintf ( name, sizeof ( name ), "sharesviewscreen_numbers %d", i );

			EclRemoveButton ( date );
			EclRemoveButton ( name );

		}

		EclRemoveButton ( "sharesviewscreen_owned" );
		EclRemoveButton ( "sharesviewscreen_profit" );
		EclRemoveButton ( "sharesviewscreen_sell" );
		EclRemoveButton ( "sharesviewscreen_buy" );

		EclRemoveButton ( "sharesviewscreen_close" );

	}

}

void SharesViewScreenInterface::Update ()
{
}

bool SharesViewScreenInterface::IsVisible ()
{
	
	return ( EclGetButton ( "sharesviewscreen_maintitle" ) != NULL );

}


int SharesViewScreenInterface::ScreenID ()
{

	return SCREEN_SHARESVIEWSCREEN;
		
}

GenericScreen *SharesViewScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);
	return (GenericScreen *) cs;

}


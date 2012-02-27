
#include <strstream>

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/miscutils.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/computer/computer.h"
#include "world/computer/recordbank.h"
#include "world/computer/security.h"
#include "world/computer/computerscreen/genericscreen.h"
#include "world/generator/numbergenerator.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/socialsecurityscreen_interface.h"

#include "mmgr.h"



SocialSecurityScreenInterface::SocialSecurityScreenInterface ()
{

	searchname = NULL;
	recordindex = -1;
	lastupdate = 0;

}

SocialSecurityScreenInterface::~SocialSecurityScreenInterface ()
{

	if ( searchname ) delete [] searchname;

}

void SocialSecurityScreenInterface::CommitClick ( Button *button )
{

	SocialSecurityScreenInterface *sssi = (SocialSecurityScreenInterface *) GetInterfaceScreen ( SCREEN_SOCSECSCREEN );
	UplinkAssert (sssi);

	if ( sssi->recordindex != -1 ) {

		Computer *comp = game->GetWorld ()->GetComputer ( "International Social Security Database" );
		UplinkAssert (comp);

		if ( comp->security.IsRunning_Proxy () ) {
			create_msgbox ( "Error", "Denied access by Proxy Server" );
			return;
		}

		Record *rec = comp->recordbank.GetRecord ( sssi->recordindex );
		UplinkAssert (rec);

		rec->ChangeField ( "Social Security",	EclGetButton ( "ss_num" )->caption );
		rec->ChangeField ( "D.O.B",				EclGetButton ( "ss_dob" )->caption );
		rec->ChangeField ( "Marital Status",	EclGetButton ( "ss_marital" )->caption );
		rec->ChangeField ( "Personal Status",	EclGetButton ( "ss_personal" )->caption );

		create_msgbox ( "Success", "Social Security Record updated" );

	}

}

void SocialSecurityScreenInterface::CloseClick ( Button *button )
{

	GenericScreen *gs = (GenericScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (gs);

	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( gs->nextpage, gs->GetComputer () );

}

bool SocialSecurityScreenInterface::EscapeKeyPressed ()
{

    CloseClick (NULL);
    return false;

}

void SocialSecurityScreenInterface::DetailsDraw ( Button *button, bool highlighted, bool clicked )
{

	textbutton_draw ( button, highlighted, true );

}

void SocialSecurityScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert (newcs);
	cs = newcs;

	if ( !IsVisible () ) {

		// Draw the screen titles

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "ss_maintitle" );
		EclRegisterButtonCallbacks ( "ss_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "ss_subtitle" );
		EclRegisterButtonCallbacks ( "ss_subtitle", DrawSubTitle, NULL, NULL, NULL );

		// Photo

		EclRegisterButton ( 30, 130, 128, 160, "", "Shows the person's photograph", "ss_photo" );
		button_assignbitmap ( "ss_photo", "photos/image0.tif" );

		// Details

		EclRegisterButton ( 180, 130, 200, 15, "Name", "", "ss_nametitle" );
		EclRegisterButton ( 180, 145, 200, 15, "logging on...", "", "ss_name" );
		EclRegisterButtonCallbacks ( "ss_name", DetailsDraw, NULL, NULL, NULL );

		EclRegisterButton ( 180, 170, 200, 15, "Social Security Number", "", "ss_numtitle" );
		EclRegisterButton ( 180, 185, 200, 15, "logging on...", "", "ss_num" );
		EclRegisterButtonCallbacks ( "ss_num", DetailsDraw, NULL, button_click, button_highlight );

		EclRegisterButton ( 180, 210, 200, 15, "Date of Birth", "", "ss_dobtitle" );
		EclRegisterButton ( 180, 225, 200, 15, "logging on...", "", "ss_dob" );
		EclRegisterButtonCallbacks ( "ss_dob", DetailsDraw, NULL, button_click, button_highlight );

		EclRegisterButton ( 180, 250, 200, 15, "Marital Status", "", "ss_maritaltitle" );
		EclRegisterButton ( 180, 265, 200, 15, "logging on...", "", "ss_marital" );
		EclRegisterButtonCallbacks ( "ss_marital", DetailsDraw, NULL, button_click, button_highlight );

		EclRegisterButton ( 180, 290, 200, 15, "Personal Status", "", "ss_personaltitle" );
		EclRegisterButton ( 180, 305, 200, 15, "logging on...", "", "ss_personal" );
		EclRegisterButtonCallbacks ( "ss_personal", DetailsDraw, NULL, button_click, button_highlight );

		EclMakeButtonEditable ( "ss_num" );
		EclMakeButtonEditable ( "ss_dob" );
		EclMakeButtonEditable ( "ss_marital" );
		EclMakeButtonEditable ( "ss_personal" );

		// Control buttons

		if ( game->GetInterface ()->GetRemoteInterface ()->security_level <= 2 ) {

			EclRegisterButton ( 30, 335, 128, 15, "Commit", "Commit all changes made", "ss_commit" );
			EclRegisterButtonCallback ( "ss_commit", CommitClick );

		}

		EclRegisterButton ( 30, 355, 128, 15, "Done", "Close this screen", "ss_close" );
		EclRegisterButtonCallback ( "ss_close", CloseClick );

	}

}

void SocialSecurityScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "ss_maintitle" );
		EclRemoveButton ( "ss_subtitle" );

		EclRemoveButton ( "ss_photo" );

		EclRemoveButton ( "ss_nametitle" );
		EclRemoveButton ( "ss_name" );

		EclRemoveButton ( "ss_numtitle" );
		EclRemoveButton ( "ss_num" );

		EclRemoveButton ( "ss_dobtitle" );
		EclRemoveButton ( "ss_dob" );

		EclRemoveButton ( "ss_maritaltitle" );
		EclRemoveButton ( "ss_marital" );

		EclRemoveButton ( "ss_personaltitle" );
		EclRemoveButton ( "ss_personal" );

		EclRemoveButton ( "ss_commit" );
		EclRemoveButton ( "ss_close" );

	}

}

void SocialSecurityScreenInterface::SetSearchName ( char *newsearchname )
{

	Computer *comp = game->GetWorld ()->GetComputer ( "International Social Security Database" );
	UplinkAssert (comp);

	recordindex = comp->recordbank.FindNextRecordIndexNameNotSystemAccount ();

	if ( searchname ) delete [] searchname;

	if ( recordindex != -1 )
		searchname = LowerCaseString (newsearchname);
	else
		searchname = NULL;

}

void SocialSecurityScreenInterface::UpdateScreen ()
{

	if ( IsVisible () ) {

		Computer *comp = game->GetWorld ()->GetComputer ( "International Social Security Database" );
		UplinkAssert (comp);

		Record *rec = comp->recordbank.GetRecord ( recordindex );
		char *thisname = rec->GetField ( RECORDBANK_NAME );

		Person *person = game->GetWorld ()->GetPerson ( thisname );

		// Show a photo

		if ( person ) {

			char filename [256];
			UplinkSnprintf ( filename, sizeof ( filename ), "photos/image%d.tif", person->photoindex );
			button_assignbitmap ( "ss_photo", filename );

		}
		else {

			button_assignbitmap ( "ss_photo", "photos/image0.tif" );

		}

		// Update display with the current record

		if ( person ) {

			char *socsecnum		= rec->GetField ( "Social Security" );
			char *dob			= rec->GetField ( "D.O.B" );
			char *marital		= rec->GetField ( "Marital Status" );
			char *personal		= rec->GetField ( "Personal Status" );

			EclGetButton ( "ss_name" )->SetCaption ( thisname );
			if ( socsecnum )
				EclGetButton ( "ss_num" )->SetCaption ( socsecnum );
			else
				EclGetButton ( "ss_num" )->SetCaption ( "" );
			if ( dob )
				EclGetButton ( "ss_dob" )->SetCaption ( dob );
			else
				EclGetButton ( "ss_dob" )->SetCaption ( "" );
			if ( marital )
				EclGetButton ( "ss_marital" )->SetCaption ( marital );
			else
				EclGetButton ( "ss_marital" )->SetCaption ( "" );
			if ( personal )
				EclGetButton ( "ss_personal" )->SetCaption ( personal );
			else
				EclGetButton ( "ss_personal" )->SetCaption ( "" );

		}

	}

}

void SocialSecurityScreenInterface::Update ()
{

	if ( searchname && IsVisible () ) {

		int timems = (int) ( EclGetAccurateTime () - lastupdate );

		if ( timems > 100 ) {

			// We are searching for a record

			// Is this one it?

			Computer *comp = game->GetWorld ()->GetComputer ( "International Social Security Database" );
			UplinkAssert (comp);

			// Have we searched all records?

			//if ( recordindex == comp->recordbank.records.Size () ) {
			if ( recordindex == -1 ) {

				delete [] searchname;
				searchname = NULL;
				recordindex = -1;
				return;

			}

			Record *rec = comp->recordbank.GetRecord ( recordindex );
			char *thisname = rec->GetField ( RECORDBANK_NAME );
			UplinkAssert (thisname);
			char *lowercasethisname = LowerCaseString (thisname);

			// Update display with the current record

			UpdateScreen ();


			if ( strcmp ( searchname, lowercasethisname ) == 0 ) {

				// Record found - stop searching

				delete [] searchname;
				searchname = NULL;

			}
			else {

				// Record not found

				//recordindex++;
				recordindex = comp->recordbank.FindNextRecordIndexNameNotSystemAccount ( recordindex );

			}

			delete [] lowercasethisname;
			lastupdate = (int) ( EclGetAccurateTime () );

		}

	}

}

bool SocialSecurityScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "ss_photo" ) != NULL );

}


int SocialSecurityScreenInterface::ScreenID ()
{

	return SCREEN_SOCSECSCREEN;

}


GenericScreen *SocialSecurityScreenInterface::GetComputerScreen ()
{

	return (GenericScreen *) cs;

}

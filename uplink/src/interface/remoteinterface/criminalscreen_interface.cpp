
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
#include "world/scheduler/arrestevent.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/criminalscreen_interface.h"

#include "mmgr.h"

CriminalScreenInterface::CriminalScreenInterface ()
{

	searchname = NULL;
	recordindex = -1;
	lastupdate = 0;

}

CriminalScreenInterface::~CriminalScreenInterface ()
{

	if ( searchname ) delete [] searchname;

}

bool CriminalScreenInterface::EscapeKeyPressed ()
{

    CloseClick (NULL);
    return true;

}

void CriminalScreenInterface::HistoryDraw ( Button *button, bool highlighted, bool clicked )
{

	textbutton_draw ( button, highlighted, true );

}

void CriminalScreenInterface::NameDraw ( Button *button, bool highlighted, bool clicked )
{

    clear_draw ( button->x, button->y, button->width, button->height );
    DrawMainTitle ( button, highlighted, clicked );

}

void CriminalScreenInterface::NameBorderDraw ( Button *button, bool highlighted, bool clicked )
{

    border_draw ( button );

}

void CriminalScreenInterface::CloseClick ( Button *button )
{

	GenericScreen *gs = (GenericScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (gs);

	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( gs->nextpage, gs->GetComputer () );

}

void CriminalScreenInterface::AddConvictionClick ( Button *button )
{

	CriminalScreenInterface *csi = (CriminalScreenInterface *) GetInterfaceScreen ( SCREEN_CRIMINALSCREEN );
	UplinkAssert (csi);

	if ( csi->recordindex != -1 ) {

		char *newconviction = EclGetButton ( "criminal_newconvictiontext" )->caption;

		Computer *comp = game->GetWorld ()->GetComputer ( "Global Criminal Database" );
		UplinkAssert (comp);

		if ( comp->security.IsRunning_Proxy () ) {
			create_msgbox ( "Error", "Denied access by Proxy Server" );
			return;
		}

		Record *rec = comp->recordbank.GetRecord ( csi->recordindex );
		UplinkAssert (rec);

		char *existing = rec->GetField ( "Convictions" );
		//UplinkAssert (existing);
		
		if ( existing ) {

			std::ostrstream newconvictions;

			if ( strstr ( existing, "None" ) == NULL )
				newconvictions << existing << "\n";

			newconvictions << newconviction
						   << '\x0';

			rec->ChangeField ( "Convictions", newconvictions.str () );

			newconvictions.rdbuf()->freeze( 0 );
			//delete [] newconvictions.str ();

		}

		EclGetButton ( "criminal_newconvictiontext" )->SetCaption ( "" );

		csi->UpdateScreen ();

	}

}

void CriminalScreenInterface::ClearClick ( Button *button )
{

	CriminalScreenInterface *csi = (CriminalScreenInterface *) GetInterfaceScreen ( SCREEN_CRIMINALSCREEN );
	UplinkAssert (csi);

	if ( csi->recordindex != -1 ) {

		Computer *comp = game->GetWorld ()->GetComputer ( "Global Criminal Database" );
		UplinkAssert (comp);

		if ( comp->security.IsRunning_Proxy () ) {
			create_msgbox ( "Error", "Denied access by Proxy Server" );
			return;
		}

		Record *rec = comp->recordbank.GetRecord ( csi->recordindex );
		UplinkAssert (rec);
		rec->ChangeField ( "Convictions", "None" );

		csi->UpdateScreen ();

	}

}

void CriminalScreenInterface::ArrestClick ( Button *button )
{

	CriminalScreenInterface *csi = (CriminalScreenInterface *) GetInterfaceScreen ( SCREEN_CRIMINALSCREEN );
	UplinkAssert (csi);

	if ( csi->recordindex != -1 ) {

		Computer *comp = game->GetWorld ()->GetComputer ( "Global Criminal Database" );
		UplinkAssert (comp);

		if ( comp->security.IsRunning_Proxy () ) {
			create_msgbox ( "Error", "Denied access by Proxy Server" );
			return;
		}

		Record *rec = comp->recordbank.GetRecord ( csi->recordindex );
		UplinkAssert (rec);

		char *name = rec->GetField ( RECORDBANK_NAME );
		UplinkAssert (name);

		char *convictions = rec->GetField ( "Convictions" );
		//UplinkAssert (convictions);

		// Only do this if the person now has at least 3 convicions
		// AND has broken parole

		int numconvictions = 0;
		bool brokenparole = false;

		if ( convictions ) {

			char *current = strchr ( convictions, '\n' );

			while ( current ) {
				++numconvictions;
				current = strchr ( current+1, '\n' );
			}

			brokenparole = strstr ( convictions, "parole" ) || strstr ( convictions, "Parole" );
		}

		if ( numconvictions >= 2 && brokenparole ) {

			// This person is going down

			Date rundate;
			rundate.SetDate ( &(game->GetWorld ()->date) );
			rundate.AdvanceMinute ( TIME_LEGALACTION );

			ArrestEvent *ae = new ArrestEvent ();
			ae->SetName ( name );
			ae->SetReason ( "breaking parole after 2 previous convictions" );
			ae->SetRunDate ( &rundate );
			game->GetWorld ()->scheduler.ScheduleEvent ( ae );

			rundate.AdvanceMinute ( TIME_LEGALACTION_WARNING * -1 );
			game->GetWorld ()->scheduler.ScheduleWarning ( ae, &rundate );

			char message [128];
			UplinkSnprintf ( message, sizeof ( message ), "Authorisation Accepted\nThis man will be arrested in %d hours.", TIME_LEGALACTION / 60 );

			create_msgbox ( "Arrest Authorised", message );

		}
		else {

			create_msgbox ( "Arrest Not Authorised", "Authorisation rejected\nA suspect must have 2 prior convictions "
													 "and must have broken parole before an arrest can be authorised remotely." );

		}

	}

}

void CriminalScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert (newcs);
	cs = newcs;

	if ( !IsVisible () ) {

		// Draw the screen titles

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "criminalscreen_maintitle" );
		EclRegisterButtonCallbacks ( "criminalscreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "criminalscreen_subtitle" );
		EclRegisterButtonCallbacks ( "criminalscreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

		EclRegisterButton ( 30, 130, 128, 160, "", "Shows the person's photograph", "criminal_photo" );
		button_assignbitmap ( "criminal_photo", "photos/image0.tif" );

        EclRegisterButton ( 180, 130, 210, 15, "Name", " ", "criminal_nametitle" );
        EclRegisterButtonCallbacks ( "criminal_nametitle", button_draw, NULL, NULL, NULL );

        EclRegisterButton ( 190, 146, 190, 38, " ", " ", "criminal_name" );
        EclRegisterButtonCallbacks ( "criminal_name", NameDraw, NULL, NULL, NULL );

        EclRegisterButton ( 180, 145, 210, 40, " ", " ", "criminal_nameborder" );
        EclRegisterButtonCallbacks ( "criminal_nameborder", NameBorderDraw, NULL, NULL, NULL );

		// History text box

		EclRegisterButton ( 180, 200, 210, 15, "Criminal History", "", "criminal_historytitle" );
		EclRegisterButtonCallbacks ( "criminal_historytitle", button_draw, NULL, NULL, NULL );

		EclRegisterButton ( 180, 215, 210, 140, "", "", "criminal_history" );
		EclRegisterButtonCallbacks ( "criminal_history", HistoryDraw, NULL, NULL, NULL );

		if ( game->GetInterface ()->GetRemoteInterface ()->security_level <= 2 ) {

			EclRegisterButton ( 180, 354, 180, 16, "", "Enter new convictions here", "criminal_newconvictiontext" );
			EclRegisterButtonCallbacks ( "criminal_newconvictiontext", HistoryDraw, NULL, NULL, button_highlight );

			EclRegisterButton ( 360, 355, 30, 15, "Add", "Click to add this conviction", "criminal_newconvictionadd" );
			EclRegisterButtonCallback ( "criminal_newconvictionadd", AddConvictionClick );

			// Control buttons

			EclRegisterButton ( 30, 315, 128, 15, "Clear history", "Click here to clear this man's record", "criminal_clear" );
			EclRegisterButtonCallback ( "criminal_clear", ClearClick );

			EclRegisterButton ( 30, 335, 128, 15, "Authorise Arrest", "Click here to authorise this man's arrest", "criminal_arrest" );
			EclRegisterButtonCallback ( "criminal_arrest", ArrestClick );

		}

		EclRegisterButton ( 30, 355, 128, 15, "Done", "Close this screen", "criminal_close" );
		EclRegisterButtonCallback ( "criminal_close", CloseClick );

        EclMakeButtonEditable ( "criminal_newconvictiontext" );

	}

}

void CriminalScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "criminalscreen_maintitle" );
		EclRemoveButton ( "criminalscreen_subtitle" );

		EclRemoveButton ( "criminal_photo" );

		EclRemoveButton ( "criminal_name" );
        EclRemoveButton ( "criminal_nameborder" );
        EclRemoveButton ( "criminal_nametitle" );

		EclRemoveButton ( "criminal_historytitle" );
		EclRemoveButton ( "criminal_history" );
		EclRemoveButton ( "criminal_newconvictiontext" );
		EclRemoveButton ( "criminal_newconvictionadd" );

		EclRemoveButton ( "criminal_clear" );
		EclRemoveButton ( "criminal_arrest" );
		EclRemoveButton ( "criminal_close" );

	}

}

void CriminalScreenInterface::SetSearchName ( char *newsearchname )
{

	Computer *comp = game->GetWorld ()->GetComputer ( "Global Criminal Database" );
	UplinkAssert (comp);

	recordindex = comp->recordbank.FindNextRecordIndexNameNotSystemAccount ();

	if ( searchname ) delete [] searchname;

	if ( recordindex != -1 )
		searchname = LowerCaseString (newsearchname);
	else
		searchname = NULL;

}

void CriminalScreenInterface::UpdateScreen ()
{

	if ( IsVisible () ) {

		Computer *comp = game->GetWorld ()->GetComputer ( "Global Criminal Database" );
		UplinkAssert (comp);

		Record *rec = comp->recordbank.GetRecord ( recordindex );
		char *thisname = rec->GetField ( RECORDBANK_NAME );
		char *history = rec->GetField ( "Convictions" );

		Person *person = game->GetWorld ()->GetPerson ( thisname );

		// Update display with the current record

		if ( thisname ) EclGetButton ( "criminal_name" )->SetCaption ( thisname );
		if ( history ) EclGetButton ( "criminal_history" )->SetCaption ( history );

		// Show a photo

		if ( person ) {

			char filename [256];
			UplinkSnprintf ( filename, sizeof ( filename ), "photos/image%d.tif", person->photoindex );
			button_assignbitmap ( "criminal_photo", filename );

		}
		else {

			button_assignbitmap ( "criminal_photo", "photos/image0.tif" );

		}

	}

}

void CriminalScreenInterface::Update ()
{

	if ( searchname && IsVisible () ) {

		int timems = (int)EclGetAccurateTime () - lastupdate;

		if ( timems > 100 ) {

			// We are searching for a record

			// Is this one it?

			Computer *comp = game->GetWorld ()->GetComputer ( "Global Criminal Database" );
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
			lastupdate = (int)EclGetAccurateTime ();

		}

	}

}

bool CriminalScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "criminal_photo" ) != NULL );

}


int CriminalScreenInterface::ScreenID ()
{

	return SCREEN_CRIMINALSCREEN;

}


GenericScreen *CriminalScreenInterface::GetComputerScreen ()
{

	return (GenericScreen *) cs;

}

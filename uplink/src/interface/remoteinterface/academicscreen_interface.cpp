
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
#include "interface/remoteinterface/academicscreen_interface.h"

#include "mmgr.h"



AcademicScreenInterface::AcademicScreenInterface ()
{

	searchname = NULL;
	recordindex = -1;
	lastupdate = 0;

}

AcademicScreenInterface::~AcademicScreenInterface ()
{

	if ( searchname ) delete [] searchname;

}

bool AcademicScreenInterface::EscapeKeyPressed ()
{

    CloseClick (NULL);
    return false;

}

void AcademicScreenInterface::CommitClick ( Button *button )
{

	AcademicScreenInterface *asi = (AcademicScreenInterface *) GetInterfaceScreen ( SCREEN_ACADEMICSCREEN );
	UplinkAssert (asi);

	if ( asi->recordindex != -1 ) {

		Computer *comp = game->GetWorld ()->GetComputer ( "International Academic Database" );
		UplinkAssert (comp);

		if ( comp->security.IsRunning_Proxy () ) {
			create_msgbox ( "Error", "Denied access by Proxy Server" );
			return;
		}

		Record *rec = comp->recordbank.GetRecord ( asi->recordindex );
		UplinkAssert (rec);

		rec->ChangeField ( "College", EclGetButton ( "academic_collegequals" )->caption );
		rec->ChangeField ( "University", EclGetButton ( "academic_uniquals" )->caption );
		rec->ChangeField ( "Other", EclGetButton ( "academic_otherquals" )->caption );

		create_msgbox ( "Success", "Academic record updated" );

	}

}

void AcademicScreenInterface::CloseClick ( Button *button )
{

	GenericScreen *gs = (GenericScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (gs);

	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( gs->nextpage, gs->GetComputer () );

}

void AcademicScreenInterface::DetailsDraw ( Button *button, bool highlighted, bool clicked )
{

	textbutton_draw ( button, highlighted, true );

}

void AcademicScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert (newcs);
	cs = newcs;

	if ( !IsVisible () ) {

		// Draw the screen titles

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "academic_maintitle" );
		EclRegisterButtonCallbacks ( "academic_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "academic_subtitle" );
		EclRegisterButtonCallbacks ( "academic_subtitle", DrawSubTitle, NULL, NULL, NULL );

		// Photo

		EclRegisterButton ( 30, 130, 128, 160, "", "Shows the person's photograph", "academic_photo" );
		button_assignbitmap ( "academic_photo", "photos/image0.tif" );

		// Details

		EclRegisterButton ( 180, 130, 200, 15, "Name", "", "academic_name" );
		EclRegisterButtonCallbacks ( "academic_name", DetailsDraw, NULL, NULL, NULL );

		EclRegisterButton ( 180, 150, 200, 15, "College Qualifications", "", "academic_collegequalstitle" );
		EclRegisterButton ( 180, 165, 200, 105, "", "", "academic_collegequals" );
		EclRegisterButtonCallbacks ( "academic_collegequals", DetailsDraw, NULL, NULL, button_highlight );

		EclRegisterButton ( 180, 270, 200, 15, "Graduate Qualifications", "", "academic_uniqualstitle" );
		EclRegisterButton ( 180, 285, 200, 30, "", "", "academic_uniquals" );
		EclRegisterButtonCallbacks ( "academic_uniquals", DetailsDraw, NULL, NULL, button_highlight );

		EclRegisterButton ( 180, 325, 200, 15, "Other Qualifications", "", "academic_otherqualstitle" );
		EclRegisterButton ( 180, 340, 200, 30, "", "", "academic_otherquals" );
		EclRegisterButtonCallbacks ( "academic_otherquals", DetailsDraw, NULL, NULL, button_highlight );

		// Control buttons

		if ( game->GetInterface ()->GetRemoteInterface ()->security_level <= 2 ) {

			EclRegisterButton ( 30, 335, 128, 15, "Commit", "Commit all changes made", "academic_commit" );
			EclRegisterButtonCallback ( "academic_commit", CommitClick );

		}

		EclRegisterButton ( 30, 355, 128, 15, "Done", "Close this screen", "academic_close" );
		EclRegisterButtonCallback ( "academic_close", CloseClick );

		EclMakeButtonEditable ( "academic_collegequals" );
		EclMakeButtonEditable ( "academic_uniquals" );
		EclMakeButtonEditable ( "academic_otherquals" );

	}

}

void AcademicScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "academic_maintitle" );
		EclRemoveButton ( "academic_subtitle" );

		EclRemoveButton ( "academic_photo" );

		EclRemoveButton ( "academic_name" );
		EclRemoveButton ( "academic_collegequalstitle" );
		EclRemoveButton ( "academic_collegequals" );
		EclRemoveButton ( "academic_uniqualstitle" );
		EclRemoveButton ( "academic_uniquals" );
		EclRemoveButton ( "academic_otherqualstitle" );
		EclRemoveButton ( "academic_otherquals" );

		EclRemoveButton ( "academic_commit" );
		EclRemoveButton ( "academic_close" );

	}

}

void AcademicScreenInterface::SetSearchName ( char *newsearchname )
{

	Computer *comp = game->GetWorld ()->GetComputer ( "International Academic Database" );
	UplinkAssert (comp);

	recordindex = comp->recordbank.FindNextRecordIndexNameNotSystemAccount ();

	if ( searchname ) delete [] searchname;

	if ( recordindex != -1 )
		searchname = LowerCaseString (newsearchname);
	else
		searchname = NULL;

}

void AcademicScreenInterface::UpdateScreen ()
{

	if ( IsVisible () ) {

		Computer *comp = game->GetWorld ()->GetComputer ( "International Academic Database" );
		UplinkAssert (comp);

		Record *rec = comp->recordbank.GetRecord ( recordindex );
		char *thisname = rec->GetField ( RECORDBANK_NAME );

		Person *person = game->GetWorld ()->GetPerson ( thisname );

		// Show a photo

		if ( person ) {

			char filename [256];
			UplinkSnprintf ( filename, sizeof ( filename ), "photos/image%d.tif", person->photoindex );
			button_assignbitmap ( "academic_photo", filename );

		}
		else {

			button_assignbitmap ( "academic_photo", "photos/image0.tif" );

		}

		// Update display with the current record

		if ( person ) {

			char *collegequals = rec->GetField ( "College" );
			char *uniquals     = rec->GetField ( "University" );
			char *otherquals   = rec->GetField ( "Other" );

			EclGetButton ( "academic_name" )->SetCaption ( thisname );
			if ( collegequals )
				EclGetButton ( "academic_collegequals" )->SetCaption ( collegequals );
			else
				EclGetButton ( "academic_collegequals" )->SetCaption ( "" );
			if ( uniquals )
				EclGetButton ( "academic_uniquals" )->SetCaption ( uniquals );
			else
				EclGetButton ( "academic_uniquals" )->SetCaption ( "" );
			if ( otherquals )
				EclGetButton ( "academic_otherquals" )->SetCaption ( otherquals );
			else
				EclGetButton ( "academic_otherquals" )->SetCaption ( "" );

		}

	}

}

void AcademicScreenInterface::Update ()
{

	if ( searchname && IsVisible () ) {

		int timems = (int)EclGetAccurateTime () - lastupdate;

		if ( timems > 100 ) {

			// We are searching for a record

			// Is this one it?

			Computer *comp = game->GetWorld ()->GetComputer ( "International Academic Database" );
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
			char *lowercasethisname = LowerCaseString ( thisname );

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

bool AcademicScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "academic_photo" ) != NULL );

}


int AcademicScreenInterface::ScreenID ()
{

	return SCREEN_ACADEMICSCREEN;

}


GenericScreen *AcademicScreenInterface::GetComputerScreen ()
{

	return (GenericScreen *) cs;

}

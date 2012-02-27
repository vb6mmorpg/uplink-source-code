

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/computerscreen/genericscreen.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/companyinfoscreen_interface.h"

#include "mmgr.h"


CompanyInfoScreenInterface::CompanyInfoScreenInterface ()
{
}

CompanyInfoScreenInterface::~CompanyInfoScreenInterface ()
{
}

bool CompanyInfoScreenInterface::EscapeKeyPressed ()
{

    BackClick (NULL);
    return true;

}

void CompanyInfoScreenInterface::MdAddClick ( Button *button )
{

	CompanyInfoScreenInterface *thisint = (CompanyInfoScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

	char *companyname = thisint->GetComputerScreen ()->GetComputer ()->companyname;
	UplinkAssert (companyname);
	Company *company = game->GetWorld ()->GetCompany ( companyname );
	UplinkAssert (company);

	Person *boss = game->GetWorld ()->GetPerson ( company->boss );
	
	if ( boss && strcmp ( boss->phonenumber, " " ) != 0 )
		game->GetWorld ()->GetPlayer ()->GiveLink ( boss->phonenumber );

	EclRemoveButton ( button->name );

}

void CompanyInfoScreenInterface::AdminAddClick ( Button *button )
{

	CompanyInfoScreenInterface *thisint = (CompanyInfoScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

	char *companyname = thisint->GetComputerScreen ()->GetComputer ()->companyname;
	UplinkAssert (companyname);
	Company *company = game->GetWorld ()->GetCompany ( companyname );
	UplinkAssert (company);

	Person *admin = game->GetWorld ()->GetPerson ( company->administrator );
	
	if ( admin && strcmp ( admin->phonenumber, " " ) != 0 )
		game->GetWorld ()->GetPlayer ()->GiveLink ( admin->phonenumber );

	EclRemoveButton ( button->name );

}

void CompanyInfoScreenInterface::BackClick ( Button *button )
{

	GenericScreen *gs = (GenericScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
    UplinkAssert (gs);

    if ( gs->nextpage != -1 )
        game->GetInterface ()->GetRemoteInterface ()->RunScreen ( gs->nextpage, gs->GetComputer () );

}

static char *getNameEmail ( const char *name )
{

	size_t nameLen = strlen ( name );
	char *nameEmail = new char [ nameLen + 1 ];
	UplinkSafeStrcpy ( nameEmail, name );

	for ( size_t i = 0; i < nameLen; i++ )
		if ( nameEmail [ i ] == ' ' || nameEmail [ i ] == '.' || nameEmail [ i ] == '\'' )
			nameEmail [ i ] = '_';

	return nameEmail;

}

void CompanyInfoScreenInterface::Create ()
{

	Create (cs);

}

void CompanyInfoScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert (newcs);
	cs = newcs;

	if ( !IsVisible () ) {

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "companyscreen_maintitle" );
		EclRegisterButtonCallbacks ( "companyscreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "companyscreen_subtitle" );
		EclRegisterButtonCallbacks ( "companyscreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

		char *companyname = GetComputerScreen ()->GetComputer ()->companyname;
		UplinkAssert (companyname);
		Company *company = game->GetWorld ()->GetCompany ( companyname );
		UplinkAssert (company);

		char mdemail [128];
		char adminemail [128];
		char mdtel [32];
		char admintel [32];

		Person *boss = game->GetWorld ()->GetPerson ( company->boss );
		Person *admin = game->GetWorld ()->GetPerson ( company->administrator );
		
		if ( boss && admin ) {

			char *companynameEmail = getNameEmail ( companyname );
			char *bossnameEmail = getNameEmail ( company->boss );
			char *administratornameEmail = getNameEmail ( company->administrator );

			UplinkSnprintf ( mdemail, sizeof ( mdemail ), "%s@%s.net", bossnameEmail, companynameEmail );
			UplinkSnprintf ( adminemail, sizeof ( adminemail ), "%s@%s.net", administratornameEmail, companynameEmail );
			UplinkSnprintf ( mdtel, sizeof ( mdtel ), "tel. %s", boss->phonenumber );
			UplinkSnprintf ( admintel, sizeof ( admintel ), "tel. %s", admin->phonenumber );

			delete [] administratornameEmail;
			delete [] bossnameEmail;
			delete [] companynameEmail;

		}
		else {

			UplinkStrncpy ( mdemail, "Unlisted", sizeof ( mdemail ) );
			UplinkStrncpy ( adminemail, "Unlisted", sizeof ( adminemail ) );
			UplinkStrncpy ( mdtel, "Unlisted", sizeof ( mdtel ) );
			UplinkStrncpy ( admintel, "Unlisted", sizeof ( admintel ) );

		}

		EclRegisterButton ( 40, 120, 300, 25, "Contact details", " ", "companyscreen_contactdetails" );
		EclRegisterButtonCallbacks ( "companyscreen_contactdetails", DrawMainTitle, NULL, NULL, NULL );

		EclRegisterButton ( 50, 150, 150, 15, "MANAGING DIRECTOR", " ", "companyscreen_mdtitle" );
		EclRegisterButton ( 50, 170, 150, 15, company->boss, " ", "companyscreen_md" );
		EclRegisterButton ( 50, 190, 250, 15, mdemail, " ", "companyscreen_mdemail" );
		EclRegisterButton ( 50, 210, 150, 15, mdtel, " ", "companyscreen_mdtel" );

		if ( boss && !game->GetWorld ()->GetPlayer ()->HasLink ( boss->phonenumber ) ) {
			EclRegisterButton ( 35, 211, 13, 13, " ", "Add this phone number to your contact list", "companyscreen_mdadd" );
			button_assignbitmaps ( "companyscreen_mdadd", "add.tif", "add_h.tif", "add_c.tif" );
			EclRegisterButtonCallback ( "companyscreen_mdadd", MdAddClick );
		}

		EclRegisterButton ( 300, 150, 150, 15, "ADMINISTRATOR", " ", "companyscreen_admintitle" );
		EclRegisterButton ( 300, 170, 150, 15, company->administrator, " ", "companyscreen_admin" );
		EclRegisterButton ( 300, 190, 250, 15, adminemail, " ", "companyscreen_adminemail" );
		EclRegisterButton ( 300, 210, 150, 15, admintel, " ", "companyscreen_admintel" );

		if ( admin && !game->GetWorld ()->GetPlayer ()->HasLink ( admin->phonenumber ) ) {
			EclRegisterButton ( 285, 211, 13, 13, " ", "Add this phone number to your contact list", "companyscreen_adminadd" );
			button_assignbitmaps ( "companyscreen_adminadd", "add.tif", "add_h.tif", "add_c.tif" );
			EclRegisterButtonCallback ( "companyscreen_adminadd", AdminAddClick );
		}

		EclRegisterButtonCallbacks ( "companyscreen_mdtitle", textbutton_draw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "companyscreen_md", textbutton_draw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "companyscreen_mdemail", textbutton_draw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "companyscreen_mdtel", textbutton_draw, NULL, NULL, NULL );

		EclRegisterButtonCallbacks ( "companyscreen_admintitle", textbutton_draw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "companyscreen_admin", textbutton_draw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "companyscreen_adminemail", textbutton_draw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "companyscreen_admintel", textbutton_draw, NULL, NULL, NULL );

        if ( GetComputerScreen ()->nextpage != -1 ) {

            EclRegisterButton ( 300, 250, 60, 15, "Back", "Return to the main menu", "companyscreen_back" );
            EclRegisterButtonCallback ( "companyscreen_back", BackClick );

        }

	}

}

void CompanyInfoScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "companyscreen_maintitle" );
		EclRemoveButton ( "companyscreen_subtitle" );

		EclRemoveButton ( "companyscreen_contactdetails" );

		EclRemoveButton ( "companyscreen_mdtitle" );
		EclRemoveButton ( "companyscreen_md" );
		EclRemoveButton ( "companyscreen_mdemail" );
		EclRemoveButton ( "companyscreen_mdtel" );
		EclRemoveButton ( "companyscreen_mdadd" );

		EclRemoveButton ( "companyscreen_admintitle" );
		EclRemoveButton ( "companyscreen_admin" );
		EclRemoveButton ( "companyscreen_adminemail" );
		EclRemoveButton ( "companyscreen_admintel" );
		EclRemoveButton ( "companyscreen_adminadd" );

        EclRemoveButton ( "companyscreen_back" );

	}

}

void CompanyInfoScreenInterface::Update ()
{

}

bool CompanyInfoScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "companyscreen_maintitle" ) != NULL );

}


int CompanyInfoScreenInterface::ScreenID ()
{

	return SCREEN_COMPANYINFO;

}

GenericScreen *CompanyInfoScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);
	return (GenericScreen *) cs;

}



#include <strstream>

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/miscutils.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/person.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/recordbank.h"
#include "world/computer/security.h"
#include "world/computer/computerscreen/genericscreen.h"
#include "world/generator/numbergenerator.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/nameserverscreen_interface.h"

#include "mmgr.h"


Image *NameServerScreenInterface::iadd_tif = NULL;
Image *NameServerScreenInterface::iadd_h_tif = NULL;
Image *NameServerScreenInterface::iadd_c_tif = NULL;

NameServerScreenInterface::NameServerScreenInterface ()
{

	searchname = NULL;
	recordindex = -1;
	lastupdate = 0;

	iadd_tif = get_assignbitmap ( "add.tif" );
	iadd_h_tif = get_assignbitmap ( "add_h.tif" );
	iadd_c_tif = get_assignbitmap ( "add_c.tif" );

}

NameServerScreenInterface::~NameServerScreenInterface ()
{

	if ( searchname ) delete [] searchname;

	if ( iadd_tif ) {
		delete iadd_tif;
		iadd_tif = NULL;
	}
	if ( iadd_h_tif ) {
		delete iadd_h_tif;
		iadd_h_tif = NULL;
	}
	if ( iadd_c_tif ) {
		delete iadd_c_tif;
		iadd_c_tif = NULL;
	}

}

bool NameServerScreenInterface::EscapeKeyPressed ()
{

    CloseClick (NULL);
    return false;

}

void NameServerScreenInterface::CommitClick ( Button *button )
{

	NameServerScreenInterface *asi = (NameServerScreenInterface *) GetInterfaceScreen ( SCREEN_NAMESERVERSCREEN );
	UplinkAssert (asi);

	if ( asi->recordindex != -1 ) {

		ComputerScreen *hostscreen = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
		UplinkAssert (hostscreen);
		Computer *comp = hostscreen->GetComputer ();
		UplinkAssert (comp);

		if ( comp->security.IsRunning_Proxy () ) {
			create_msgbox ( "Error", "Denied access by Proxy Server" );
			return;
		}

		Record *rec = comp->recordbank.GetRecord ( asi->recordindex );
		UplinkAssert (rec);

		rec->ChangeField ( "Owner", EclGetButton ( "nameserver_owner" )->caption );
		rec->ChangeField ( "Admin", EclGetButton ( "nameserver_admin" )->caption );
		rec->ChangeField ( "Tech", EclGetButton ( "nameserver_tech" )->caption );
		rec->ChangeField ( "NS1", EclGetButton ( "nameserver_nameserver1" )->caption );
		rec->ChangeField ( "NS2", EclGetButton ( "nameserver_nameserver2" )->caption );

		create_msgbox ( "Success", "DNS record updated" );
	}

}

void NameServerScreenInterface::CloseClick ( Button *button )
{

	GenericScreen *gs = (GenericScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (gs);

	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( gs->nextpage, gs->GetComputer () );

}

void NameServerScreenInterface::IPClick ( Button *button )
{

	char buttonsuffix[64];
	char unused[64];
	sscanf(button->name, "%s %s", unused, buttonsuffix);

	char name[128];
	UplinkSnprintf(name, sizeof(name), "nameserver_%s", buttonsuffix);

	Button *ipbutton = EclGetButton(name);
	UplinkAssert(ipbutton);

	VLocation *vl = game->GetWorld ()->GetVLocation ( ipbutton->caption );
	if ( vl )
	{
		if ( !game->GetWorld ()->GetPlayer ()->HasLink ( ipbutton->caption ) )
			game->GetWorld ()->GetPlayer ()->GiveLink ( ipbutton->caption );
	}

}

void NameServerScreenInterface::AddLinkDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert ( button );
	char buttonsuffix[64];
	char unused[64];
	sscanf(button->name, "%s %s", unused, buttonsuffix);

	char name[128];
	UplinkSnprintf(name, sizeof(name), "nameserver_%s", buttonsuffix);

	Button *ipbutton = EclGetButton(name);
	if ( ipbutton )
	{
		VLocation *vl = game->GetWorld ()->GetVLocation ( ipbutton->caption );
		if ( vl )
		{
			if ( !game->GetWorld ()->GetPlayer ()->HasLink (ipbutton->caption) ) 
				imagebutton_draw ( button, highlighted, clicked );

			else
				clear_draw ( button->x, button->y, button->width, button->height );
		}
		else
			clear_draw ( button->x, button->y, button->width, button->height );
	}
}

void NameServerScreenInterface::DetailsDraw ( Button *button, bool highlighted, bool clicked )
{

	textbutton_draw ( button, highlighted, true );

}

void NameServerScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert (newcs);
	cs = newcs;

	if ( !IsVisible () ) {

		// Draw the screen titles

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "nameserver_maintitle" );
		EclRegisterButtonCallbacks ( "nameserver_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "nameserver_subtitle" );
		EclRegisterButtonCallbacks ( "nameserver_subtitle", DrawSubTitle, NULL, NULL, NULL );

		// Details

		EclRegisterButton (  30, 130, 350, 15, "Name", "", "nameserver_nametitle" );
		EclRegisterButton (  30, 145, 350, 15, "", "", "nameserver_name" );
		EclRegisterButtonCallbacks ( "nameserver_name", DetailsDraw, NULL, NULL, button_highlight );

		EclRegisterButton (  30, 165, 170, 15, "IP Address", "", "nameserver_iptitle" );
		EclRegisterButton (  30, 180, 155, 15, "", "", "nameserver_ip" );
		EclRegisterButtonCallbacks ( "nameserver_ip", DetailsDraw, NULL, NULL, button_highlight );

		EclRegisterButton ( 186, 181, 13, 13, "", "Store this link", "nameserver_addlink ip" );
		button_assignbitmaps ( "nameserver_addlink ip", iadd_tif, iadd_h_tif, iadd_c_tif );
		EclRegisterButtonCallbacks ( "nameserver_addlink ip", AddLinkDraw, IPClick, button_click, button_highlight );

		EclRegisterButton ( 30, 200, 170, 15, "Owner", "", "nameserver_ownertitle" );
		EclRegisterButton ( 30, 215, 170, 15, "", "", "nameserver_owner" );
		EclRegisterButtonCallbacks ( "nameserver_owner", DetailsDraw, NULL, NULL, button_highlight );

		EclRegisterButton ( 210, 165, 170, 15, "Administrative Contact", "", "nameserver_admintitle" );
		EclRegisterButton ( 210, 180, 170, 15, "", "", "nameserver_admin" );
		EclRegisterButtonCallbacks ( "nameserver_admin", DetailsDraw, NULL, NULL, button_highlight );

		EclRegisterButton ( 210, 200, 170, 15, "Technical Contact", "", "nameserver_techtitle" );
		EclRegisterButton ( 210, 215, 170, 15, "", "", "nameserver_tech" );
		EclRegisterButtonCallbacks ( "nameserver_tech", DetailsDraw, NULL, NULL, button_highlight );

		EclRegisterButton (  30, 235, 350, 15, "Nameservers", "", "nameserver_nameserverstitle" );

		EclRegisterButton (  30, 250, 100, 15, "", "", "nameserver_nameserver1" );
		EclRegisterButtonCallbacks ( "nameserver_nameserver1", DetailsDraw, NULL, NULL, button_highlight );
		EclRegisterButton ( 130, 250, 235, 15, "", "", "nameserver_nameserver1a" );
		EclRegisterButtonCallbacks ( "nameserver_nameserver1a", DetailsDraw, NULL, NULL, button_highlight );
		EclRegisterButton ( 366, 251, 13, 13, "", "Store this link", "nameserver_addlink nameserver1" );
		button_assignbitmaps ( "nameserver_addlink nameserver1", iadd_tif, iadd_h_tif, iadd_c_tif );
		EclRegisterButtonCallbacks ( "nameserver_addlink nameserver1", AddLinkDraw, IPClick, button_click, button_highlight );

		EclRegisterButton (  30, 265, 100, 15, "", "", "nameserver_nameserver2" );
		EclRegisterButtonCallbacks ( "nameserver_nameserver2", DetailsDraw, NULL, NULL, button_highlight );
		EclRegisterButton ( 130, 265, 235, 15, "", "", "nameserver_nameserver2a" );
		EclRegisterButtonCallbacks ( "nameserver_nameserver2a", DetailsDraw, NULL, NULL, button_highlight );
		EclRegisterButton ( 366, 266, 13, 13, "", "Store this link", "nameserver_addlink nameserver2" );
		button_assignbitmaps ( "nameserver_addlink nameserver2", iadd_tif, iadd_h_tif, iadd_c_tif );
		EclRegisterButtonCallbacks ( "nameserver_addlink nameserver2", AddLinkDraw, IPClick, button_click, button_highlight );

		// Control buttons

		if ( game->GetInterface ()->GetRemoteInterface ()->security_level <= 2 ) {

			EclRegisterButton ( 30, 335, 128, 15, "Commit", "Commit all changes made", "nameserver_commit" );
			EclRegisterButtonCallback ( "nameserver_commit", CommitClick );

		}

		EclRegisterButton ( 30, 355, 128, 15, "Done", "Close this screen", "nameserver_close" );
		EclRegisterButtonCallback ( "nameserver_close", CloseClick );

		EclMakeButtonEditable ( "nameserver_nameserver1" );
		EclMakeButtonEditable ( "nameserver_nameserver2" );
		EclMakeButtonEditable ( "nameserver_owner" );
		EclMakeButtonEditable ( "nameserver_admin" );
		EclMakeButtonEditable ( "nameserver_tech" );

	}

}

void NameServerScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "nameserver_maintitle" );
		EclRemoveButton ( "nameserver_subtitle" );

		EclRemoveButton ( "nameserver_nametitle" );
		EclRemoveButton ( "nameserver_name" );
		EclRemoveButton ( "nameserver_iptitle" );
		EclRemoveButton ( "nameserver_ip" );
		EclRemoveButton ( "nameserver_nameserverstitle" );
		EclRemoveButton ( "nameserver_nameserver1" );
		EclRemoveButton ( "nameserver_nameserver2" );
		EclRemoveButton ( "nameserver_nameserver1a" );
		EclRemoveButton ( "nameserver_nameserver2a" );
		EclRemoveButton ( "nameserver_ownertitle" );
		EclRemoveButton ( "nameserver_owner" );
		EclRemoveButton ( "nameserver_admintitle" );
		EclRemoveButton ( "nameserver_admin" );
		EclRemoveButton ( "nameserver_techtitle" );
		EclRemoveButton ( "nameserver_tech" );

		EclRemoveButton ( "nameserver_addlink ip" );
		EclRemoveButton ( "nameserver_addlink nameserver1" );
		EclRemoveButton ( "nameserver_addlink nameserver2" );

		EclRemoveButton ( "nameserver_commit" );
		EclRemoveButton ( "nameserver_close" );

	}

}

void NameServerScreenInterface::SetSearchName ( char *newsearchname )
{

	ComputerScreen *hostscreen = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (hostscreen);
	Computer *comp = hostscreen->GetComputer ();
	UplinkAssert (comp);

	recordindex = comp->recordbank.FindNextRecordIndexNameNotSystemAccount ();

	if ( searchname ) delete [] searchname;

	if ( recordindex != -1 )
		searchname = LowerCaseString (newsearchname);
	else
		searchname = NULL;

}

void NameServerScreenInterface::UpdateScreen ()
{

	if ( IsVisible () ) {

		ComputerScreen *hostscreen = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
		UplinkAssert (hostscreen);
		Computer *comp = hostscreen->GetComputer ();
		UplinkAssert (comp);

		Record *rec = comp->recordbank.GetRecord ( recordindex );
		char *thisname = rec->GetField ( RECORDBANK_NAME );

		// Update display with the current record

		char *thisip	= rec->GetField ( "IP" );
		char *owner		= rec->GetField ( "Owner" );
		char *admin		= rec->GetField ( "Admin" );
		char *tech		= rec->GetField ( "Tech" );
		char *ns1		= rec->GetField ( "NS1" );
		char *ns2		= rec->GetField ( "NS2" );

		EclGetButton ( "nameserver_name" )->SetCaption ( thisname );
		if ( thisip )
			EclGetButton ( "nameserver_ip" )->SetCaption ( thisip );
		else
			EclGetButton ( "nameserver_ip" )->SetCaption ( "Unlisted" );
		if ( owner )
			EclGetButton ( "nameserver_owner" )->SetCaption ( owner );
		else
			EclGetButton ( "nameserver_owner" )->SetCaption ( "Unlisted" );
		if ( admin )
			EclGetButton ( "nameserver_admin" )->SetCaption ( admin );
		else
			EclGetButton ( "nameserver_admin" )->SetCaption ( "Unlisted" );
		if ( tech )
			EclGetButton ( "nameserver_tech" )->SetCaption ( tech );
		else
			EclGetButton ( "nameserver_tech" )->SetCaption ( "Unlisted" );
		if ( ns1 ) {
			EclGetButton ( "nameserver_nameserver1" )->SetCaption ( ns1 );
			VLocation *vl = game->GetWorld ()->GetVLocation( ns1 );
			if ( vl ) {
				Computer *comp = vl->GetComputer();
				if ( comp )
					EclGetButton ( "nameserver_nameserver1a" )->SetCaption ( comp->name );
				else
					EclGetButton ( "nameserver_nameserver1a" )->SetCaption ( "Invalid IP" );
			}
			else
				EclGetButton ( "nameserver_nameserver1a" )->SetCaption ( "Invalid IP" );
		}
		else {
			EclGetButton ( "nameserver_nameserver1" )->SetCaption ( IP_INTERNIC );
			VLocation *vl = game->GetWorld ()->GetVLocation( IP_INTERNIC );
			if ( vl ) {
				Computer *comp = vl->GetComputer();
				if ( comp )
					EclGetButton ( "nameserver_nameserver1a" )->SetCaption ( comp->name );
				else
					EclGetButton ( "nameserver_nameserver1a" )->SetCaption ( "Invalid IP" );
			}
			else
				EclGetButton ( "nameserver_nameserver1a" )->SetCaption ( "Invalid IP" );
		}
		if ( ns2 ) {
			EclGetButton ( "nameserver_nameserver2" )->SetCaption ( ns2 );
			VLocation *vl = game->GetWorld ()->GetVLocation( ns2 );
			if ( vl ) {
				Computer *comp = vl->GetComputer();
				if ( comp )
					EclGetButton ( "nameserver_nameserver2a" )->SetCaption ( comp->name );
				else
					EclGetButton ( "nameserver_nameserver2a" )->SetCaption ( "Invalid IP" );
			}
			else
				EclGetButton ( "nameserver_nameserver2a" )->SetCaption ( "Invalid IP" );
		}
		else {
			EclGetButton ( "nameserver_nameserver2" )->SetCaption ( IP_INTERNIC );
			VLocation *vl = game->GetWorld ()->GetVLocation( IP_INTERNIC );
			if ( vl ) {
				Computer *comp = vl->GetComputer();
				if ( comp )
					EclGetButton ( "nameserver_nameserver2a" )->SetCaption ( comp->name );
				else
					EclGetButton ( "nameserver_nameserver2a" )->SetCaption ( "Invalid IP" );
			}
			else
				EclGetButton ( "nameserver_nameserver2a" )->SetCaption ( "Invalid IP" );
		}

	}

}

void NameServerScreenInterface::UpdateLinks ()
{
	Button *button = EclGetButton("nameserver_nameserver1");
	if ( button )
	{
		VLocation *vl = game->GetWorld ()->GetVLocation ( button->caption );
		if ( vl )
		{
			Computer *comp = vl->GetComputer();
			if ( comp )
				EclGetButton ( "nameserver_nameserver1a" )->SetCaption ( comp->name );
			else
				EclGetButton ( "nameserver_nameserver1a" )->SetCaption ( "Invalid IP" );
		}
		else
			EclGetButton ( "nameserver_nameserver1a" )->SetCaption ( "Invalid IP" );
	}

	button = EclGetButton("nameserver_nameserver2");
	if ( button )
	{
		VLocation *vl = game->GetWorld ()->GetVLocation ( button->caption );
		if ( vl )
		{
			Computer *comp = vl->GetComputer();
			if ( comp )
				EclGetButton ( "nameserver_nameserver2a" )->SetCaption ( comp->name );
			else
				EclGetButton ( "nameserver_nameserver2a" )->SetCaption ( "Invalid IP" );
		}
		else
			EclGetButton ( "nameserver_nameserver2a" )->SetCaption ( "Invalid IP" );
	}

}

void NameServerScreenInterface::Update ()
{

	if ( searchname && IsVisible () ) {

		int timems = (int)EclGetAccurateTime () - lastupdate;

		if ( timems > 100 ) {

			// We are searching for a record

			// Is this one it?

			ComputerScreen *hostscreen = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
			UplinkAssert (hostscreen);
			Computer *comp = hostscreen->GetComputer ();
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

			char *thisip = rec->GetField ( "IP" );
			UplinkAssert (thisip);
			char *lowercasethisip = LowerCaseString ( thisip );

			// Update display with the current record

			UpdateScreen ();


			if ( strcmp ( searchname, lowercasethisname ) == 0 ||
				 strcmp ( searchname, lowercasethisip   ) == 0 ) {

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
	UpdateLinks();

}

bool NameServerScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "nameserver_name" ) != NULL );

}


int NameServerScreenInterface::ScreenID ()
{

	return SCREEN_NAMESERVERSCREEN;

}


GenericScreen *NameServerScreenInterface::GetComputerScreen ()
{

	return (GenericScreen *) cs;

}

// connection.cpp: implementation of the Connection class.
//
//////////////////////////////////////////////////////////////////////

#include "gucci.h"

#include "game/game.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "world/world.h"
#include "world/connection.h"
#include "world/person.h"
#include "world/player.h"
#include "world/vlocation.h"
#include "world/computer/computer.h"
#include "world/computer/logbank.h"
#include "world/computer/lanmonitor.h"
#include "world/computer/computerscreen/disconnectedscreen.h"

#include "mmgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Connection::Connection()
{

	UplinkStrncpy ( owner, "", sizeof ( owner ) );
	traceinprogress = false;
	traceprogress = 0;

}

Connection::~Connection()
{
                                       
	DeleteLListData ( &vlocations );            

}

void Connection::SetOwner ( char *newowner )
{

	UplinkAssert ( strlen (newowner) < 64 );
	UplinkStrncpy ( owner, newowner, sizeof ( owner ) );

}

void Connection::AddVLocation ( char *ip )
{
	
	// First make sure this ip isn't already in there

	if ( LocationIncluded ( ip ) ) return;

	size_t newipsize = SIZE_VLOCATION_IP;
	char *newip = new char [newipsize];
	UplinkAssert ( strlen (ip) < SIZE_VLOCATION_IP );
	UplinkStrncpy ( newip, ip, newipsize );

	vlocations.PutDataAtEnd ( newip );

}

/********** Start code by François Gagné **********/
void Connection::AddOrRemoveVLocation ( char *ip )
{

	// Cannot remove the first location

	for ( int i = 1; i < vlocations.Size (); ++i )
		if ( strcmp ( vlocations.GetData (i), ip ) == 0 ) {
			delete [] vlocations.GetData (i);
			vlocations.RemoveData ( i );
			return;
		}

	AddVLocation ( ip );

}
/********** End code by François Gagné **********/

void Connection::RemoveLastLocation ()
{
	
    char *lastIP = vlocations.GetData ( vlocations.Size () - 1 );
    vlocations.RemoveData ( vlocations.Size () - 1 );
    delete [] lastIP;

}

bool Connection::LocationIncluded ( char *ip )
{

	for ( int i = 0; i < vlocations.Size (); ++i )
		if ( strcmp ( vlocations.GetData (i), ip ) == 0 )
			return true;

	return false;

}

char *Connection::GetSource ()
{

	if ( vlocations.ValidIndex ( 0 ) )
		return vlocations.GetData ( 0 );

	else
		return NULL;

}

char *Connection::GetTarget ()
{

	if ( vlocations.ValidIndex ( 0 ) )
		return vlocations.GetData ( vlocations.Size () - 1 );

	else
		return NULL;

}

char *Connection::GetGhost ()
{

	if ( vlocations.Size () > 1 )						// Must be at least 1 entry
		return vlocations.GetData ( vlocations.Size () - 2 );

	else
		return NULL;


}

Person *Connection::GetOwner ()
{

	Person *person = game->GetWorld ()->GetPerson ( owner );
	UplinkAssert ( person );

	return person;

}

int Connection::GetSize ()
{

	return vlocations.Size ();

}

void Connection::BeginTrace ()
{

	if ( !traceinprogress ) {

		// Should a trace occur?
		// Check the trace action
	
		VLocation *vl = game->GetWorld ()->GetVLocation ( GetTarget () );
		UplinkAssert (vl);
		Computer *comp = vl->GetComputer ();
		UplinkAssert (comp);

		if ( comp->traceaction > 0 ) {

			traceinprogress = true;
			traceprogress = 0;

		}

	}

}

void Connection::EndTrace ()
{

	traceinprogress = false;

}

void Connection::Connect ()
{

	GetOwner ()->SetRemoteHost ( GetTarget () );
	traceprogress = 0;
	traceinprogress = false;
	nexttrace.SetDate ( &(game->GetWorld ()->date) );

	//
	// If any security system is disabled, begin a trace immediately
	//

	VLocation *tvl = game->GetWorld ()->GetVLocation ( GetTarget () );
	UplinkAssert (tvl);
	Computer *comp = tvl->GetComputer ();
	UplinkAssert (comp);

	if ( comp->security.IsAnythingDisabled () ) BeginTrace ();

    //
    // Special case if this is the Player
    //

	if ( strcmp ( owner, "PLAYER" ) == 0 ) {

		SecurityMonitor::BeginAttack ();
        LanMonitor::BeginAttack ();

        if ( strcmp ( comp->companyname, "ARC" ) == 0 ) 
            game->GetWorld ()->plotgenerator.PlayerVisitsPlotSites ();

	}

		
	//
	// Log this action on each node in the connection
	//

	for ( int i = 0; i < vlocations.Size (); ++i ) {

		VLocation *vl = game->GetWorld ()->GetVLocation ( vlocations.GetData (i) );
		UplinkAssert (vl);

		Computer *bouncecomp = vl->GetComputer ();

		if ( bouncecomp ) {

			if ( i == vlocations.Size () - 1 ) {

				// Previous -> This (ie connection opened)
				AccessLog *log = new AccessLog ();
				log->SetProperties ( &(game->GetWorld ()->date), vlocations.GetData (i-1), owner, 
										LOG_NOTSUSPICIOUS, LOG_TYPE_CONNECTIONOPENED );
				bouncecomp->logbank.AddLog ( log );

			}		
			else {			

				// Previous -> Next (ie a bounce)
				AccessLog *log = new AccessLog ();
			
				int logtype = ( i == 0 ) ? LOG_TYPE_BOUNCEBEGIN : LOG_TYPE_BOUNCE;
				char *fromip = ( i == 0 ) ? (char *) "LOCAL" : vlocations.GetData (i-1);

				log->SetProperties ( &(game->GetWorld ()->date), fromip, owner,
									  LOG_NOTSUSPICIOUS, logtype );
				log->SetData1 ( vlocations.GetData (i+1) );

				bouncecomp->logbank.AddLog ( log );

			}

		}

	}

}

void Connection::Disconnect ()
{

	/*
	   Log this action on the final node in the connection
	   This is a "suspicious" action if the trace was started but not completed
	   (A trace is only started if the person is hacking - and if it finished 
	   then he has already been caught - no need to be suspicious	   

	   */

	VLocation *vl = game->GetWorld ()->GetVLocation ( GetTarget () );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert ( comp );

	int suspicious = ( TraceInProgress () && !Traced () ) ?
						LOG_SUSPICIOUS	 :
						LOG_NOTSUSPICIOUS;

	AccessLog *log1 = new AccessLog ();
	log1->SetProperties ( &(game->GetWorld ()->date), 
						  vlocations.GetData ( vlocations.Size () - 2 ), owner,
						  LOG_NOTSUSPICIOUS, LOG_TYPE_CONNECTIONCLOSED );

	/*
	   If it was suspicious, look for previous "connection opened" logs 
	   made by this person and make them suspicious instead
	   (There should be only 1 suspicious log left behind for this hack)
	  
		If it wasn't suspicious, just add a standard "conenction closed" log

		*/
	
	if ( !suspicious ) {

		comp->logbank.AddLog ( log1 );
		
	}
	else {

		bool found = false;

		for ( int i = comp->logbank.logs.Size (); i >= 0; --i ) {
			if ( comp->logbank.logs.ValidIndex (i) ) {

				AccessLog *al = comp->logbank.logs.GetData (i);
				UplinkAssert (al);

				Date testdate;
				testdate.SetDate ( &al->date );
				testdate.AdvanceMinute ( 5 );

				if ( testdate.Before ( &log1->date ) ) break;					// Every other log is > 5 mins before

				if ( al->TYPE == LOG_TYPE_CONNECTIONOPENED &&
					 strcmp ( al->fromname, owner ) == 0 &&				
					 strcmp ( al->fromip, log1->fromip ) == 0 ) {
					
					al->SetSuspicious ( LOG_SUSPICIOUS );

					if ( comp->logbank.internallogs.ValidIndex (i) )
						comp->logbank.internallogs.GetData (i)->SetSuspicious ( LOG_SUSPICIOUS );

					// Add in the connection closed log, not suspicious
					comp->logbank.AddLog ( log1 );

					found = true;
					break;

				}

			}
		}

		if ( !found ) {

			// Didn't find a connection opened log, so add in the 
			// connection closed log as suspicious

			log1->SetSuspicious ( LOG_SUSPICIOUS );
			comp->logbank.AddLog ( log1 );

		}

	}	


	//
	// Do the disconnect
	//

	if ( strcmp ( owner, "PLAYER" ) == 0 ) {

		SecurityMonitor::EndAttack ();
        LanMonitor::EndAttack ();


        // Change admin passwords if trace is more than 50% complete

        int rumbledindex = traceprogress - (GetSize () - 1) / 2;

        if ( rumbledindex >= 0 ) {

            for ( int i = rumbledindex; i >= 0; --i ) {

                char *rumbledIP = vlocations.GetData (GetSize () - i - 1);
                UplinkAssert (rumbledIP);

                if ( game->GetWorld ()->GetPlayer ()->HasAccount (rumbledIP) != -1 ) {

                    VLocation *vl = game->GetWorld ()->GetVLocation (rumbledIP);
                    UplinkAssert (vl);
                    Computer *comp = vl->GetComputer ();
                    UplinkAssert (comp);

                    bool changed = comp->ChangeSecurityCodes ();

                    if ( changed ) DisconnectedScreen::AddLoginLost ( comp->ip );

                }

            }

        }

	}

	GetOwner ()->SetRemoteHost ( GetOwner ()->localhost );	
	traceprogress = 0;
	traceinprogress = false;

}

void Connection::Reset ()
{
	
	DeleteLListData ( &vlocations );
    vlocations.Empty ();

    AddVLocation ( GetOwner ()->localhost );

	traceinprogress = false;
	traceprogress = 0;

}

bool Connection::TraceInProgress ()
{

	return traceinprogress;

}

bool Connection::Traced ()
{

	return ( traceinprogress &&
			 traceprogress == GetSize () - 1 );

}

bool Connection::Connected ()
{

	return ( strcmp ( GetOwner ()->localhost, GetOwner ()->remotehost ) != 0 );

}

bool Connection::Load ( FILE *file )
{

	LoadID ( file );

	if ( strcmp( game->GetLoadedSavefileVer(), "SAV59" ) >= 0 ) {
		if ( !LoadDynamicStringStatic ( owner, sizeof(owner), file ) ) return false;
	}
	else {
		if ( !FileReadData ( owner, sizeof(owner), 1, file ) ) {
			owner [ sizeof(owner) - 1 ] = '\0';
			return false;
		}
		owner [ sizeof(owner) - 1 ] = '\0';
	}
	if ( !FileReadData ( &traceinprogress, sizeof(traceinprogress), 1, file ) ) return false;
	if ( !FileReadData ( &traceprogress, sizeof(traceprogress), 1, file ) ) return false;

	if ( !LoadLList ( &vlocations, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void Connection::Save ( FILE *file )
{

	SaveID ( file );

	SaveDynamicString ( owner, sizeof(owner), file );
	fwrite ( &traceinprogress, sizeof(traceinprogress), 1, file );
	fwrite ( &traceprogress, sizeof(traceprogress), 1, file );

	SaveLList ( &vlocations, file );

	SaveID_END ( file );

}

void Connection::Print ()
{

	printf ( "Connection : Owner = %s, TraceInProgress = %d, TraceProgress = %d\n", owner, traceinprogress, traceprogress );
	PrintLList ( &vlocations );

}
	
char *Connection::GetID ()
{
	
	return "CONNECT";

}



#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"

#include "game/game.h"

#include "world/world.h"
#include "world/vlocation.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/securitysystem.h"
#include "world/computer/securitymonitor.h"

#include "mmgr.h"


int SecurityMonitor::status = 0;

DArray <bool> SecurityMonitor::currentmonitor;

Date SecurityMonitor::nextmonitor_date;
int SecurityMonitor::nextmonitor_index = 0;


void SecurityMonitor::BeginAttack ()
{

	// Look up the computer system the player is connected to

	VLocation *vl = game->GetWorld ()->GetVLocation ( game->GetWorld ()->GetPlayer ()->remotehost );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

	// Enable monitering if there are systems to be monitered

	if ( comp->security.NumSystems () > 0 ) {

		currentmonitor.SetSize ( comp->security.NumSystems () );
		for ( int i = 0; i < currentmonitor.Size (); ++i ) 
			currentmonitor.PutData ( false, i );

		nextmonitor_date.SetDate ( &(game->GetWorld ()->date) );
		nextmonitor_date.AdvanceSecond ( 30 / comp->security.NumSystems () );

		nextmonitor_index = 0;

		status = 1;

	}

}

void SecurityMonitor::EndAttack ()
{

	currentmonitor.Empty ();

	status = 0;

}

bool SecurityMonitor::IsMonitored ( int index )
{

	if ( status == 2 )
		if ( currentmonitor.ValidIndex (index) )
			if ( currentmonitor.GetData (index) == true )
				return true;

	return false;

}

	
void SecurityMonitor::Update ()
{

	switch ( status ) {

		case 0 :									// Not enabled

			break;

		case 1 :									// Enabled, waiting to begin monitoring

			if ( game->GetWorld ()->date.After ( &nextmonitor_date ) ) {

				VLocation *vl = game->GetWorld ()->GetVLocation ( game->GetWorld ()->GetPlayer ()->remotehost );
				UplinkAssert (vl);
				Computer *comp = vl->GetComputer ();
				UplinkAssert (comp);

				if ( comp->security.IsRunning_Monitor () ) {

					// Time up - begin monitering the next system

					currentmonitor.PutData ( true, nextmonitor_index );

					// You'll be rumbled in ~ 4 seconds

					nextmonitor_date.SetDate ( &(game->GetWorld ()->date) );
					nextmonitor_date.AdvanceSecond ( 4 );					

					status = 2;

				}
				else {

					nextmonitor_date.SetDate ( &(game->GetWorld ()->date) );
					if ( comp->security.NumSystems () > 0 )
						nextmonitor_date.AdvanceSecond ( 30 / comp->security.NumSystems () );
					else
						nextmonitor_date.AdvanceSecond ( 30 );

				}

			}
			break;

		case 2 :									// Monitering
			
			if ( game->GetWorld ()->date.After ( &nextmonitor_date ) ) {

				VLocation *vl = game->GetWorld ()->GetVLocation ( game->GetWorld ()->GetPlayer ()->remotehost );
				UplinkAssert (vl);
				Computer *comp = vl->GetComputer ();
				UplinkAssert (comp);

				if ( comp->security.IsRunning_Monitor () ) {

					// Time up - if player has jimmied this system then he 
					// is now caught

					for ( int i = 0; i < currentmonitor.Size (); ++i ) {
						if ( currentmonitor.ValidIndex (i) ) {
							if ( currentmonitor.GetData (i) == true ) {

								SecuritySystem *ss = comp->security.GetSystem (i);
								UplinkAssert (ss);

								if ( ss->bypassed || !ss->enabled ) 
									game->GetWorld ()->GetPlayer ()->GetConnection ()->BeginTrace ();
								
							}
						}
					}

				}
				
				// Reset and wait for a few seconds before analysing the next system

				++nextmonitor_index;
				if ( !comp->security.GetSystem (nextmonitor_index) ) nextmonitor_index = 0;

				for ( int ic = 0; ic < currentmonitor.Size (); ++ic ) 
					currentmonitor.PutData ( false, ic );
					
				nextmonitor_date.SetDate ( &(game->GetWorld ()->date) );
				if ( comp->security.NumSystems () > 0 )
					nextmonitor_date.AdvanceSecond ( 30 / comp->security.NumSystems () );
				else
					nextmonitor_date.AdvanceSecond ( 30 );

				status = 1;

			}
			break;

		default:

			UplinkAbort ( "Unrecognised status\n" );
			break;

	}

}

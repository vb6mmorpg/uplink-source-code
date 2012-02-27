
#include "gucci.h"
#include "eclipse.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"

#include "world/player.h"
#include "world/world.h"
#include "world/vlocation.h"
#include "world/computer/computer.h"
#include "world/computer/lanmonitor.h"
#include "world/computer/lancomputer.h"
#include "world/company/companyuplink.h"

#include "world/generator/worldgenerator.h"
#include "world/generator/numbergenerator.h"
#include "world/generator/missiongenerator.h"

#include "world/scheduler/eventscheduler.h"
#include "world/scheduler/attemptmissionevent.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"

#include "mmgr.h"


bool LanMonitor::currentlyActive = false;
LanComputer *LanMonitor::lanComputer = NULL;

int LanMonitor::currentSystem = -1;
int LanMonitor::currentSelected = -1;
int LanMonitor::currentSpoof = -1;

int LanMonitor::sysAdminState = -1;
int LanMonitor::sysAdminCurrentSystem = -1;
int LanMonitor::sysAdminTimer = -1;


LList <int> LanMonitor::connection;



void LanMonitor::BeginAttack ()
{

    Player *player = game->GetWorld ()->GetPlayer ();
    UplinkAssert (player);
	VLocation *tvl = game->GetWorld ()->GetVLocation ( player->remotehost );
	UplinkAssert (tvl);
	Computer *comp = tvl->GetComputer ();
	UplinkAssert (comp);
    
	currentSystem = -1;
	currentSelected = -1;
	currentSpoof = -1;

    sysAdminState = SYSADMIN_ASLEEP;
    sysAdminCurrentSystem = -1;

    if ( comp->TYPE == COMPUTER_TYPE_LAN ) {

        ResetAll ();                                        // Just to be safe

        lanComputer = (LanComputer *) comp;
        currentlyActive = true;

        int ourFirstLink = -1;

        if ( tvl->GetOBJECTID () == OID_VLOCATIONSPECIAL ) 
            ourFirstLink = ((VLocationSpecial *) tvl)->securitySystemIndex;
                
        if ( ourFirstLink == -1 ) {

            //
		    // Insert the Router to the connection

            for ( int k = 0; k < lanComputer->systems.Size(); ++k ) {
                if ( lanComputer->systems.ValidIndex(k) ) {
                    LanComputerSystem *system = lanComputer->systems.GetData(k);
				    UplinkAssert (system);
				    if ( system->TYPE == LANSYSTEM_ROUTER ) {
                        ourFirstLink = k;
                        break;
				    }
			    }
		    }
        }

        UplinkAssert ( ourFirstLink != -1 );
        UplinkAssert ( lanComputer->systems.ValidIndex ( ourFirstLink ) );

  		connection.PutDataAtStart( ourFirstLink );
        currentSystem = ourFirstLink;
        currentSelected = ourFirstLink;

        lanComputer->systems.GetData( ourFirstLink )->IncreaseVisibility( LANSYSTEMVISIBLE_TYPE );

    }
    else {

        currentlyActive = false;

    }
  
}


void LanMonitor::EndAttack ()
{

    ResetAll ();

}

void LanMonitor::ResetAll ()
{

	connection.Empty ();

    currentlyActive = false;
    lanComputer = NULL;
	currentSystem = -1;
	currentSelected = -1;
	currentSpoof = -1;

    EclSuperUnHighlight ( "hud_lanview" );

}

void LanMonitor::SetCurrentSystem ( int newCurrentSystem )
{
	currentSystem = newCurrentSystem;

	//
	// Reset our security level

	game->GetInterface ()->GetRemoteInterface ()->SetSecurity( "Guest", 10 );

}

void LanMonitor::SetCurrentSelected ( int newCurrentSelected )
{
	currentSelected = newCurrentSelected;
}

void LanMonitor::SetCurrentSpoof ( int newCurrentSpoof )
{
	currentSpoof = newCurrentSpoof;
}

void LanMonitor::ExtendConnection ( int newNode )
{

    if ( !IsAccessable (newNode) )
        return;

	//
	// Is it adjacent to an existing linked system, 
	// with a visible link to this one?

	int linkHeadIndex = connection.Size() - 1;
	if ( !connection.ValidIndex(linkHeadIndex) )
		return;

	int linkHead = connection.GetData(linkHeadIndex);
	
	for ( int i = 0; i < lanComputer->links.Size(); ++i ) {
		if ( lanComputer->links.ValidIndex(i) ) {
		
			LanComputerLink *link = lanComputer->links.GetData(i);
			UplinkAssert(link);

			if ( ( (link->from == linkHead && link->to == newNode) ||
                   (link->to == linkHead && link->from == newNode) ) &&
			 	 link->visible == LANLINKVISIBLE_AWARE ) {

				if ( lanComputer->systems.ValidIndex ( newNode ) ) {
					connection.PutDataAtEnd( newNode );
					lanComputer->systems.GetData( newNode )->IncreaseVisibility( LANSYSTEMVISIBLE_TYPE );
				}

				break;

			}

		}
	}

}

void LanMonitor::ForceExtendConnection ( int newNode )
{

    //
    // Look up the new system

	UplinkAssert ( lanComputer );

	if ( !lanComputer->systems.ValidIndex(newNode) ) 
		return;

    connection.PutDataAtEnd ( newNode );
    lanComputer->systems.GetData( newNode )->IncreaseVisibility( LANSYSTEMVISIBLE_TYPE );

}

void LanMonitor::RetractConnection ()
{

    int removeIndex = connection.Size() - 1;
    if ( removeIndex > 0 ) {
        connection.RemoveData( removeIndex );
        currentSelected = connection.GetData( connection.Size() - 1 );
    }

}

bool LanMonitor::IsInConnection ( int node )
{

    return ( GetNodeIndex (node) != -1 );

}

int LanMonitor::GetNodeIndex ( int node )
{

	for ( int i = 0; i < connection.Size(); ++i ) {
		if ( connection.ValidIndex(i) ) {

			int thisNode = connection.GetData(i);
			if ( thisNode == node )
				return i;

		}
	}

	return -1;

}

bool LanMonitor::IsAccessable ( int newNode )
{
    
	//
	// Look up the new system

	if ( !currentlyActive ) 
		return false;

	UplinkAssert ( lanComputer );

	if ( !lanComputer->systems.ValidIndex(newNode) ) 
		return false;

	LanComputerSystem *newSystem = lanComputer->systems.GetData(newNode);
	UplinkAssert (newSystem);

	
	// 
	// Is it visible / known about to the player?

	if ( newSystem->visible == LANSYSTEMVISIBLE_NONE ||
		 newSystem->visible == LANSYSTEMVISIBLE_AWARE )
		 return false;

    //
    // Is the system an 'island'?

    if ( newNode != currentSystem ) {

        bool knownLinks = false;
        for ( int i = 0; i < lanComputer->links.Size(); ++i ) {
            if ( lanComputer->links.ValidIndex(i) ) {
                LanComputerLink *link = lanComputer->links.GetData(i);
                UplinkAssert (link);
                
                if ( (link->to == newNode && link->visible == LANLINKVISIBLE_TOAWARE) ||
                     (link->from == newNode && link->visible == LANLINKVISIBLE_FROMAWARE) ) {
                     knownLinks = true;
                     break;
                }
            
                if ( (link->to == newNode || link->from == newNode) &&
                    link->visible == LANLINKVISIBLE_AWARE ) {
                    knownLinks = true;
                    break;
                }

            }
        }

        if ( !knownLinks ) return false;

    }

	//
	// Is it a locked lock ?

	if ( newSystem->TYPE == LANSYSTEM_LOCK &&
		 newSystem->data1 == 1 )
		return false;

    // 
    // If it is a Server, all locks must be unlocked

    if ( newSystem->TYPE == LANSYSTEM_MAINSERVER ) {

        if ( newSystem->data1 != -1 && lanComputer->systems.ValidIndex ( newSystem->data1 ) && lanComputer->systems.GetData(newSystem->data1)->data1 == 1 ) return false;
        if ( newSystem->data2 != -1 && lanComputer->systems.ValidIndex ( newSystem->data2 ) && lanComputer->systems.GetData(newSystem->data2)->data1 == 1 ) return false;
        if ( newSystem->data3 != -1 && lanComputer->systems.ValidIndex ( newSystem->data3 ) && lanComputer->systems.GetData(newSystem->data3)->data1 == 1 ) return false;

    }

    //
    // If it has restricted FROM addresses,
    // are we coming from one of them?

    if ( newSystem->validSubnets.Size() > 0 ) {
        int playerSubnet = currentSpoof;
        if ( currentSpoof == -1 ) return false;
        
        bool valid = false;
        for ( int i = 0; i < newSystem->validSubnets.Size(); ++i ) {
			if ( newSystem->validSubnets.ValidIndex ( i ) ) {
				int thisSubnet = newSystem->validSubnets.GetData(i);
				if ( thisSubnet == currentSpoof )
					valid = true;
			}
        }

        return valid;
    }

    return true;

}

void LanMonitor::SysAdminAwaken ()
{

    if ( sysAdminState < SYSADMIN_CURIOUS ) {

        sysAdminState = SYSADMIN_CURIOUS;
        int timeToDiscover = (int) NumberGenerator::RandomNormalNumber( 10, 5 );
        sysAdminTimer = (int) ( EclGetAccurateTime() + timeToDiscover * 1000 );
    
    }
    
}

void LanMonitor::SysAdminSleep ()
{
    sysAdminState = SYSADMIN_ASLEEP;
    sysAdminCurrentSystem = -1;
}

void LanMonitor::Update ()
{

    if ( sysAdminState < SYSADMIN_CURIOUS && EclIsSuperHighlighted( "hud_lanview" ) ) {

        EclSuperUnHighlight ( "hud_lanview" );

    }
    else if ( sysAdminState >= SYSADMIN_CURIOUS && !EclIsSuperHighlighted ( "hud_lanview" ) ) {

        EclSuperHighlight ( "hud_lanview" );
        SgPlaySound ( RsArchiveFileOpen ( "sounds/siren.wav" ), "sounds/sirens.wav" );

    }

    
    if ( !currentlyActive ) return;

    //
    // Update the system administrator

    switch ( sysAdminState )
    {

        case SYSADMIN_ASLEEP:
        {
            
            if ( lanComputer && lanComputer->systems.ValidIndex( currentSystem ) &&
                ( lanComputer->systems.GetData( currentSystem )->TYPE == LANSYSTEM_MAINSERVER ||
                  lanComputer->systems.GetData( currentSystem )->TYPE == LANSYSTEM_FILESERVER ||
                  lanComputer->systems.GetData( currentSystem )->TYPE == LANSYSTEM_LOGSERVER ) ) {

                SysAdminAwaken ();

            }

        }
        break;

        case SYSADMIN_CURIOUS:
        {
            if ( EclGetAccurateTime() >= sysAdminTimer ) {
                sysAdminState = SYSADMIN_SEARCHING;
                if ( connection.ValidIndex(0) ) 
                    sysAdminCurrentSystem = connection.GetData(0);
                else 
                    sysAdminState = SYSADMIN_ASLEEP;                    
                int timeToDiscover = (int) NumberGenerator::RandomNormalNumber( 10, 5 );
                sysAdminTimer = (int) ( EclGetAccurateTime() + timeToDiscover * 1000 );
                game->GetWorld ()->GetPlayer ()->GetConnection ()->BeginTrace ();
            }
        }
        break;

        case SYSADMIN_SEARCHING:
        {
            if ( sysAdminCurrentSystem == currentSystem ) {
                sysAdminState = SYSADMIN_FOUNDYOU;
            }
            else {

                if ( EclGetAccurateTime() >= sysAdminTimer ) {
                    int nodeIndex = GetNodeIndex( sysAdminCurrentSystem );
                    if ( nodeIndex == -1 ) 
                        sysAdminState = SYSADMIN_CURIOUS;
                    else {
                        if ( connection.ValidIndex(nodeIndex+1) )
                            sysAdminCurrentSystem = connection.GetData(nodeIndex+1);
                        else
                            sysAdminState = SYSADMIN_CURIOUS;
                    }
                    int timeToDiscover = (int) NumberGenerator::RandomNormalNumber( 10, 5 );
                    sysAdminTimer = (int) ( EclGetAccurateTime() + timeToDiscover * 1000 );
                }

            }

        }    
        break;

        case SYSADMIN_FOUNDYOU:
        {
        
            // Find an agent who will perform a Trace mission on you
            // in a few seconds

            Agent *agent = WorldGenerator::GetRandomAgent ();
            UplinkAssert (agent);

            Mission *mission = MissionGenerator::Generate_TraceHacker( lanComputer, game->GetWorld ()->GetPlayer () );
            agent->missions.PutDataAtStart( mission );	

            // Remove the mission from Uplink's list

            CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
            UplinkAssert (cu);
            for ( int i = 0; i < cu->missions.Size(); ++i ) {
                if ( cu->missions.ValidIndex(i) ) {
                    if ( cu->missions.GetData(i) == mission ) {
                        cu->missions.RemoveData( i );
                        break;
                    }
                }
            }

		    // Schedule an event to make the agent attempt the mission
		    // in a few seconds

		    Date duedate;
		    duedate.SetDate ( &game->GetWorld ()->date );
		    duedate.AdvanceSecond ( (int) NumberGenerator::RandomNormalNumber ( 30, 10 ) );

		    AttemptMissionEvent *ami = new AttemptMissionEvent ();
		    ami->SetAgentName ( agent->name );
		    ami->SetRunDate ( &duedate );
		    game->GetWorld ()->scheduler.ScheduleEvent ( ami );

            sysAdminState = SYSADMIN_ASLEEP;
            sysAdminCurrentSystem = -1;
            sysAdminTimer = 0;

            //
            // Dump the player out

			game->GetWorld ()->GetPlayer ()->GetConnection ()->Disconnect ();
			game->GetWorld ()->GetPlayer ()->GetConnection ()->Reset ();
			game->GetInterface ()->GetRemoteInterface ()->RunNewLocation ();
			game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 2);		
            
        }
        break;

    }

}

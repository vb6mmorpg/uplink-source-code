// Computer.cpp: implementation of the Computer class.
//
//////////////////////////////////////////////////////////////////////

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/vlocation.h"
#include "world/player.h"
#include "world/company/mission.h"
#include "world/computer/computer.h"
#include "world/computer/logbank.h"
#include "world/computer/recordbank.h"
#include "world/computer/security.h"
#include "world/computer/securitysystem.h"
#include "world/computer/computerscreen/computerscreen.h"

#include "world/generator/namegenerator.h"
#include "world/generator/worldgenerator.h"
#include "world/generator/numbergenerator.h"
#include "world/generator/consequencegenerator.h"
#include "world/generator/newsgenerator.h"
#include "world/generator/missiongenerator.h"

#include "world/scheduler/notificationevent.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"

#include "mmgr.h"


Computer::Computer()
{

	TYPE = COMPUTER_TYPE_UNKNOWN;
	UplinkStrncpy ( name, "", sizeof ( name ) );
	UplinkStrncpy ( companyname, "", sizeof ( companyname ) );
	UplinkStrncpy ( ip, "", sizeof ( ip ) );
	tracespeed = 0;
	traceaction = COMPUTER_TRACEACTION_NONE;

	numrecenthacks = 0;
	numhacksthismonth = 0;
	numhackslastmonth = 0;

	istargetable = true;
	isexternallyopen = true;
	isrunning = true;
	isinfected_revelation = 0.0;

}

Computer::~Computer()
{

	DeleteDArrayData ( (DArray <UplinkObject *> *) &screens );

}

void Computer::SetTYPE ( int newTYPE )
{

	TYPE = newTYPE;

}

void Computer::SetName ( char *newname )
{

	UplinkAssert ( strlen ( newname ) < SIZE_COMPUTER_NAME );
	UplinkStrncpy ( name, newname, sizeof ( name ) );

}

void Computer::SetCompanyName ( char *newname )
{

	if ( newname ) {

		UplinkAssert ( strlen ( newname ) < SIZE_COMPANY_NAME );
		UplinkAssert ( game->GetWorld ()->GetCompany ( newname ) );
		UplinkStrncpy ( companyname, newname, sizeof ( companyname ) );

	}

}

void Computer::SetIP ( char *newip )
{

	UplinkAssert ( strlen (newip) < SIZE_VLOCATION_IP );
	UplinkAssert ( game->GetWorld ()->GetVLocation ( newip ) );
	UplinkStrncpy ( ip, newip, sizeof ( ip ) );

}

void Computer::SetTraceSpeed ( int newtracespeed )
{
	
	tracespeed = newtracespeed;

}

void Computer::SetTraceAction ( int newtraceaction )
{

	traceaction = newtraceaction;

}

void Computer::SetIsTargetable ( bool value )
{

	istargetable = value;

}

void Computer::SetIsExternallyOpen	( bool value )
{

	isexternallyopen = value;

}

void Computer::SetIsRunning ( bool value )
{

	isrunning = value;

}

void Computer::DisinfectRevelation ()
{

    if ( isinfected_revelation > 0.0 )
        game->GetWorld ()->plotgenerator.Disinfected ( ip );

	isinfected_revelation = 0.0;

}

void Computer::InfectWithRevelation ( float version )
{

	isinfected_revelation = version;
    infectiondate.SetDate ( &game->GetWorld ()->date );

    if ( version <= 1.0 ) {

        // Destroy the computer immediately

	    SetIsRunning ( false );
	    databank.Format ();
	    
        logbank.Empty ();

    }

}

int  Computer::AddComputerScreen ( ComputerScreen *cs, int index )
{

	// Give the computer a pointer to this computer screen
	UplinkAssert (cs);
	cs->SetComputer ( name );

	if ( index == -1 ) {
		
		// Place the screen in the next available slot
		return screens.PutData ( cs );

	}	
	else {

		// Place the screen at the specified position
		if ( index >= screens.Size () )
			screens.SetSize ( index + 1 );

		screens.PutData ( cs, index );
		return index;

	}

}

ComputerScreen *Computer::GetComputerScreen ( int index )
{

	if ( screens.ValidIndex ( index ) )
		return (ComputerScreen *) screens.GetData (index);

	else
		return NULL;

}

void Computer::CheckForSecurityBreaches ()
{

	bool timetochangeunderware = false;

	//
	// Has someone fucked this computer over?
	//

	if ( !isrunning ) {

		if ( !isinfected_revelation ) {

			if ( databank.formatted ) NewsGenerator::ComputerDestroyed ( this, true );
			else					  NewsGenerator::ComputerDestroyed ( this, false );

		}
		
		SetIsRunning ( true );
		DisinfectRevelation ();
		databank.formatted = false;
		
		return;

	}

	//
	// Check for suspect logs
	//

	for ( int i = 0; i < logbank.logs.Size (); ++i ) {
		if ( logbank.logs.ValidIndex (i) ) {

			AccessLog *al = logbank.logs.GetData (i);
			
			AccessLog *internallog = NULL;
			if ( logbank.internallogs.ValidIndex (i) ) internallog = logbank.internallogs.GetData (i);

			if ( al->SUSPICIOUS == LOG_SUSPICIOUS ) {

				// Now noticed - they have another 3 hours before trace

				// Special case - don't trace if this is the player, and he is performing
				// a Trace Hacker mission for this company

				bool ignoreme = false;

				if ( strcmp ( al->fromname, "PLAYER" ) == 0 ) {

					for ( int l = 0; l < game->GetWorld ()->GetPlayer ()->missions.Size (); ++l ) {
						Mission *m = game->GetWorld ()->GetPlayer ()->missions.GetData (l);
						UplinkAssert (m);

						if ( m->TYPE == MISSION_TRACEUSER && strcmp ( m->completionB, ip ) == 0 ) 
							ignoreme = true;

					}

				}

				if ( !ignoreme ) {

					al->SetSuspicious ( LOG_SUSPICIOUSANDNOTICED );
					if ( internallog ) internallog->SetSuspicious ( LOG_SUSPICIOUSANDNOTICED );
					timetochangeunderware = true;

				}
				else {

					al->SetSuspicious ( LOG_NOTSUSPICIOUS );

				}

			}
			else if ( al->SUSPICIOUS == LOG_SUSPICIOUSANDNOTICED ) {


				// Noticed and time up - pass this onto the consequence generator
				ConsequenceGenerator::ComputerHacked ( this, al );

				// No longer suspicious - now under investigation
				al->SetSuspicious ( LOG_UNDERINVESTIGATION );
				if ( internallog ) internallog->SetSuspicious ( LOG_UNDERINVESTIGATION );

			}

		}
	}

	//
	// Have all our files been wiped?
	//

	if ( databank.formatted ) {

		char *softwaretype [4] = { "SCIENTIFIC", "CORPORATE", "CUSTOMER", "SOFTWARE" };
		int softwareindex = NumberGenerator::RandomNumber ( 4 );

		NewsGenerator::AllFilesDeleted ( this, softwaretype [softwareindex] );
		databank.formatted = false;
		timetochangeunderware = true;

	}

	//
	// Reset our defenses if they have been compromised
	//
	
	if ( security.IsAnythingDisabled () ||
		timetochangeunderware ) {
    
        ChangeSecurityCodes ();

	}

}

bool Computer::ChangeSecurityCodes ()
{

	// Change our admin password

	bool changed = false;

    switch ( TYPE ) {

		case COMPUTER_TYPE_INTERNALSERVICESMACHINE :		
		{
			Record *admin = recordbank.GetRecordFromName ( RECORDBANK_ADMIN );
			UplinkAssert (admin);
			admin->ChangeField ( RECORDBANK_PASSWORD, NameGenerator::GenerateComplexPassword () );		
            changed = true;
			break;
		}
		
        case COMPUTER_TYPE_CENTRALMAINFRAME :
        {
			Record *admin = recordbank.GetRecordFromName ( RECORDBANK_ADMIN );
			UplinkAssert (admin);
			admin->ChangeField ( RECORDBANK_PASSWORD, NameGenerator::GenerateComplexPassword () );		
            changed = true;
			break;
        }

        case COMPUTER_TYPE_UNKNOWN :
        {

            if ( strcmp ( ip, IP_SOCIALSECURITYDATABASE ) == 0 ||
                 strcmp ( ip, IP_GLOBALCRIMINALDATABASE ) == 0 ||
                 strcmp ( ip, IP_ACADEMICDATABASE ) == 0 ) {

			    Record *admin = recordbank.GetRecordFromName ( RECORDBANK_ADMIN );
			    UplinkAssert (admin);
			    admin->ChangeField ( RECORDBANK_PASSWORD, NameGenerator::GenerateComplexPassword () );		
                
			    Record *readwrite = recordbank.GetRecordFromName ( RECORDBANK_READWRITE );
			    UplinkAssert (readwrite);
			    readwrite->ChangeField ( RECORDBANK_PASSWORD, NameGenerator::GenerateComplexPassword () );		

  			    Record *readonly = recordbank.GetRecordFromName ( RECORDBANK_READONLY );
			    UplinkAssert (readonly);
			    readonly->ChangeField ( RECORDBANK_PASSWORD, NameGenerator::GenerateComplexPassword () );		

                changed = true;

            }

        }

	}

	// Re-enable all our security

	for ( int i = 0; i < security.NumSystems (); ++i ) 
		if ( security.GetSystem (i) ) 
			security.GetSystem (i)->Enable ();

    return changed;

}


void Computer::ManageOldLogs ()
{

	//
	// Delete any logs older than a certain age
	//

	for ( int i = logbank.logs.Size () - 1; i >= 0; --i ) {
		if ( logbank.logs.ValidIndex (i) ) {

			AccessLog *al = logbank.logs.GetData (i);

			if ( al ) {

				Date testdate;
				testdate.SetDate ( &(al->date) );
				testdate.AdvanceMinute ( TIME_TOEXPIRELOGS );

				if ( testdate.Before ( &(game->GetWorld ()->date) ) ) {

					delete logbank.logs.GetData (i);
					logbank.logs.RemoveData (i);

					if ( logbank.internallogs.ValidIndex ( i ) ) {
						delete logbank.internallogs.GetData (i);				
						logbank.internallogs.RemoveData (i);
					}

				}

			}

		}

	}
	
	//
	// Pack the new log structures together
	//

	int nextlog = 0;

	for ( int il = 0; il < logbank.logs.Size (); ++il ) {

		if ( il > nextlog ) nextlog = il;

		if ( !logbank.logs.ValidIndex (il) ) {

			// This is a blank spot
			// Look for the next available log to fill this space
			
			bool found = false;

			for ( ; nextlog < logbank.logs.Size (); ++nextlog ) {
				
				if ( logbank.logs.ValidIndex (nextlog) ) {

					logbank.logs.PutData	( logbank.logs.GetData (nextlog), il );
					logbank.logs.RemoveData (nextlog);

					if ( logbank.internallogs.ValidIndex(nextlog) ) {
						logbank.internallogs.PutData ( logbank.internallogs.GetData (nextlog), il );
						logbank.internallogs.RemoveData (nextlog);
					}

					++nextlog;

					found = true;
					break;

				}

			}

			if ( !found ) {

				// There are no more valid logs - 
				// So resize the DArray since this is now the max size
				// Then quit

				logbank.logs.SetSize ( il );
				logbank.internallogs.SetSize ( il );
				break;

			}

		}

	}

}

void Computer::AddToRecentHacks ( int n )
{

	numrecenthacks += n;
	numhacksthismonth += n;

}

void Computer::UpdateRecentHacks ()
{

	// This is called 4 times a month 
	// Reduce the number of recent hacks by the number last month / 4
	// (removing all of last months hacks from the count by the end of the month)

	// If it is the end of the month, process the changeover to a new month
	// Also schedule the next check

	if ( game->GetWorld ()->date.GetDay () == 1 ) {

		// New month		

		numhackslastmonth = numhacksthismonth;
		numhacksthismonth = 0;

	}
	else {

		// Reduce hacks this month by a quarter

		int numtoremove = numhackslastmonth / 4;
		numrecenthacks -= numtoremove;

	}

	// Do some clamping, just to be safe

	if ( numrecenthacks < 0 ) numrecenthacks = 0;
	if ( numhackslastmonth < 0 ) numhackslastmonth = 0;
	
}

int Computer::NumRecentHacks ()
{

	return numrecenthacks;

}

bool Computer::Load  ( FILE *file )
{

	LoadID ( file );

	if ( !FileReadData ( &TYPE, sizeof(TYPE), 1, file ) ) return false;

	if ( !LoadDynamicStringStatic ( name, SIZE_COMPUTER_NAME, file ) ) return false;
	if ( !LoadDynamicStringStatic ( companyname, SIZE_COMPANY_NAME, file ) ) return false;
	if ( !LoadDynamicStringStatic ( ip, SIZE_VLOCATION_IP, file ) ) return false;

	if ( !FileReadData ( &tracespeed, sizeof(tracespeed), 1, file ) ) return false;
	if ( !FileReadData ( &traceaction, sizeof(traceaction), 1, file ) ) return false;

	if ( !FileReadData ( &numrecenthacks, sizeof(numrecenthacks), 1, file ) ) return false;
	if ( !FileReadData ( &numhacksthismonth, sizeof(numhacksthismonth), 1, file ) ) return false;
	if ( !FileReadData ( &numhackslastmonth, sizeof(numhackslastmonth), 1, file ) ) return false;

	if ( !FileReadData ( &istargetable, sizeof(istargetable), 1, file ) ) return false;
	if ( !FileReadData ( &isexternallyopen, sizeof(isexternallyopen), 1, file ) ) return false;
	if ( !FileReadData ( &isrunning, sizeof(isrunning), 1, file ) ) return false;
	if ( !FileReadData ( &isinfected_revelation, sizeof(isinfected_revelation), 1, file ) ) return false;

	if ( !LoadDArray ( (DArray <UplinkObject *> *) &screens, file ) ) return false;

	if ( !databank.Load ( file ) ) return false;
	if ( !logbank.Load ( file ) ) return false;
	if ( !recordbank.Load ( file ) ) return false;
	if ( !security.Load ( file ) ) return false;
    if ( !infectiondate.Load ( file ) ) return false;

	LoadID_END ( file );

	return true;

}

void Computer::Save  ( FILE *file )
{

	SaveID ( file );

	fwrite ( &TYPE, sizeof(TYPE), 1, file );

	SaveDynamicString ( name, SIZE_COMPUTER_NAME, file );
	SaveDynamicString ( companyname, SIZE_COMPANY_NAME, file );
	SaveDynamicString ( ip, SIZE_VLOCATION_IP, file );

	fwrite ( &tracespeed, sizeof(tracespeed), 1, file );
	fwrite ( &traceaction, sizeof(traceaction), 1, file );

	fwrite ( &numrecenthacks, sizeof(numrecenthacks), 1, file );
	fwrite ( &numhacksthismonth, sizeof(numhacksthismonth), 1, file );
	fwrite ( &numhackslastmonth, sizeof(numhackslastmonth), 1, file );

	fwrite ( &istargetable, sizeof(istargetable), 1, file );
	fwrite ( &isexternallyopen, sizeof(isexternallyopen), 1, file );
	fwrite ( &isrunning, sizeof(isrunning), 1, file );
	fwrite ( &isinfected_revelation, sizeof(isinfected_revelation), 1, file );

	SaveDArray ( (DArray <UplinkObject *> *) &screens, file );	
	
	databank.Save ( file );
	logbank.Save ( file );
	recordbank.Save ( file );
	security.Save ( file );
    infectiondate.Save ( file );

	SaveID_END ( file );

}

void Computer::Print ()
{

	printf ( "Computer : Name = %s\n", name );
	printf ( "TYPE = %d, Companyname = %s, ip = %s, tracespeed = %d, traceaction = %d, targetable = %d, externally open = %d, running = %d\n", TYPE, companyname, ip, tracespeed, traceaction, istargetable, isexternallyopen, isrunning );
	printf ( "numrecenthacks = %d, numhacksthismonth = %d, numhackslastmonth = %d\n", numrecenthacks, numhacksthismonth, numhackslastmonth );
	printf ( "infected with revelation = %f\n", isinfected_revelation );
    
    infectiondate.Print ();

	PrintDArray ( (DArray <UplinkObject *> *) &screens );	

	databank.Print ();
	logbank.Print ();
	recordbank.Print ();
	security.Print ();

}
	
void Computer::Update ()
{

	if ( !isrunning ) return;

    if ( isinfected_revelation > 1.0 ) {

        Date damagedate;
        damagedate.SetDate ( &infectiondate );
        damagedate.AdvanceMinute ( TIME_REVELATIONREPRODUCE );

        if ( game->GetWorld ()->date.After ( &damagedate ) ) {

            //
            // Spread to two other computers

            Computer *comp1 = WorldGenerator::GetRandomComputer ( COMPUTER_TYPE_INTERNALSERVICESMACHINE );
            Computer *comp2 = WorldGenerator::GetRandomComputer ( COMPUTER_TYPE_INTERNALSERVICESMACHINE |
                                                                  COMPUTER_TYPE_CENTRALMAINFRAME        );

            UplinkAssert (comp1);
            UplinkAssert (comp2);

            game->GetWorld ()->plotgenerator.RunRevelation ( comp1->ip, isinfected_revelation, false );
            game->GetWorld ()->plotgenerator.RunRevelation ( comp2->ip, isinfected_revelation, false );

            //
            // Shut down this computer

	        SetIsRunning ( false );
	        databank.Format ();
	        logbank.Empty ();
            game->GetWorld ()->plotgenerator.Disinfected ( ip );

            //
            // Disconnect the player if he is connected

            if ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, ip ) == 0 ) {

	            game->GetWorld ()->GetPlayer ()->connection.Disconnect ();
	            game->GetWorld ()->GetPlayer ()->connection.Reset ();

	            game->GetInterface ()->GetRemoteInterface ()->RunNewLocation ();
	            game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 2 );

            }
            
        }

    }

	//
	// Generate some new files
	//

	if ( databank.NumDataFiles () > 0 && 
		 NumberGenerator::RandomNumber ( 1000 ) == 0 ) {

		Data *data = new Data ();
		data->SetTitle ( NameGenerator::GenerateDataName ( "companyname", DATATYPE_DATA ) );
		data->SetDetails ( DATATYPE_DATA, NumberGenerator::RandomNumber ( 10 ) + 1, 0, 0 );
		if ( !databank.PutData ( data ) )
			delete data;

	}

	//
	// Generate some new logs
	//

	if ( NumberGenerator::RandomNumber ( 1000 ) == 0 ) {

		AccessLog *al = new AccessLog ();
		al->SetProperties ( &(game->GetWorld ()->date), WorldGenerator::GetRandomLocation ()->ip, " " );
		al->SetData1 ( "Accessed File" );
		logbank.AddLog (al);

	}

}

void Computer::GenerateAccessCode( char *code, char *result, size_t resultsize )
{
    UplinkSnprintf( result, resultsize, "%s:'%s'", "CODE", code );
}

void Computer::GenerateAccessCode( char *name, char *code, char *result, size_t resultsize )
{
    UplinkSnprintf( result, resultsize, "%s:'%s', %s:'%s'", "NAME", name, "CODE", code );
}

void Computer::GenerateAccessCode( int accNo, char *code, char *result, size_t resultsize )
{
    UplinkSnprintf( result, resultsize, "%s:'%d', %s:'%s'", "NAME", accNo, "CODE", code );
}

char *Computer::GetID ()
{

	return "COMPUT";

}

int Computer::GetOBJECTID ()
{

	return OID_COMPUTER;

}

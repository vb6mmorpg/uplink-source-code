
#include <strstream>

#include "gucci.h"

#include "app/globals.h"
#include "app/app.h"
#include "app/serialise.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/computer/computer.h"
#include "world/computer/logbank.h"

#include "mmgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LogBank::LogBank ()
{
}

LogBank::~LogBank ()
{

    DeleteDArrayData ( (DArray <UplinkObject *> *) &logs );
    DeleteDArrayData ( (DArray <UplinkObject *> *) &internallogs );

}

void LogBank::AddLog ( AccessLog *log, int index )
{

	if ( index == -1 ) index = logs.Size ();

	// Add the log on at the end

	if ( index >= logs.Size () )
		logs.SetSize ( index + 1 );

	logs.PutData ( log, index );

	// Add the log into the internal structure

	AccessLog *internalcopy = new AccessLog ();
	internalcopy->SetProperties ( log );

	internallogs.SetSize ( logs.Size () );
	internallogs.PutData ( internalcopy, index );

}

bool LogBank::LogModified ( int index )
{

	if ( !logs.ValidIndex (index) || !internallogs.ValidIndex (index) )
		return false;


	AccessLog *log = logs.GetData (index);
	AccessLog *internallog = internallogs.GetData (index);

	// Are they the same object?
	// Do a comparison

	return ( log->TYPE != internallog->TYPE ||
			 log->SUSPICIOUS != internallog->SUSPICIOUS ||
			 strcmp ( log->fromip, internallog->fromip ) != 0 ||
			 strcmp ( log->fromname, internallog->fromname ) != 0 ||
			 !log->date.Equal ( &internallog->date ) );

}

char *LogBank::TraceLog ( char *to_ip, char *logbank_ip, Date *date, int uplinkrating )
{

	UplinkAssert ( to_ip );
	UplinkAssert ( logbank_ip );
	UplinkAssert ( date );

	//
	// Get some information regarding the local machine
	//

	VLocation *vl_local = game->GetWorld ()->GetVLocation ( logbank_ip );
	UplinkAssert (vl_local);
	Computer *comp_local = vl_local->GetComputer ();
	UplinkAssert (comp_local);
	Company *company_local = game->GetWorld ()->GetCompany ( comp_local->companyname );
	UplinkAssert (company_local);

	// Try to find a log that showed a user bouncing from this machine
	// to to_ip.
	// Then recurse into that source log.

	for ( int i = 0; i < logs.Size (); ++i ) {
		if ( logs.ValidIndex (i) ) {

			AccessLog *al = logs.GetData (i);

			// If the log was deleted/overwritten we may be able to recover it

			if ( al->TYPE == LOG_TYPE_DELETED &&
			     uplinkrating >= MINREQUIREDRATING_UNDELETELOGLEVEL1 ) {

				if ( internallogs.ValidIndex (i) ) {

					AccessLog *recovered = internallogs.GetData (i);
					if ( recovered ) {
						AccessLog *internalcopy = new AccessLog ();
						internalcopy->SetProperties ( recovered );
						logs.PutData ( internalcopy, i );
						delete al;
						al = internalcopy;
					}

				}

			}
			else if ( al->TYPE != LOG_TYPE_DELETED &&
					  LogModified (i) &&
					  uplinkrating >= MINREQUIREDRATING_UNDELETELOGLEVEL3 ) {

				// This one isn't deleted but overwritten, however the origional still exists

				if ( internallogs.ValidIndex (i) ) {

					AccessLog *recovered = internallogs.GetData (i);
					if ( recovered ) {
						AccessLog *internalcopy = new AccessLog ();
						internalcopy->SetProperties ( recovered );
						logs.PutData ( internalcopy, i );
						delete al;
						al = internalcopy;
					}

				}

			}

			// Only look at the log if it is within a few seconds of the connection date
			// (Otherwise it could be from anywhere)

			Date upperdate;
			Date lowerdate;

			upperdate.SetDate ( date );
			lowerdate.SetDate ( date );

			upperdate.AdvanceSecond ( 10 );
			lowerdate.AdvanceSecond ( -10 );

			if ( al->date.After ( &lowerdate ) &&
				 al->date.Before ( &upperdate ) ) {

				// Now look at the log

				if ( al->TYPE == LOG_TYPE_BOUNCEBEGIN &&
					 al->data1 && strcmp ( al->data1, to_ip ) == 0 ) {

					// This computer is the origin of the bounced call
					// And is therefore the solution to this trace
					return logbank_ip;

				}
				else if ( al->TYPE == LOG_TYPE_BOUNCE &&
						  al->data1 && strcmp ( al->data1, to_ip ) == 0 ) {

					// Look up the source computer that created this log

					VLocation *vl = game->GetWorld ()->GetVLocation ( al->fromip );
					if ( vl ) {

						Computer *comp = vl->GetComputer ();
						if ( comp ) {

							bool isbank = comp_local->TYPE == COMPUTER_TYPE_PUBLICBANKSERVER;
							bool isgov  = company_local->TYPE == COMPANYTYPE_GOVERNMENT;

							if ( (!isbank || (isbank && uplinkrating >= MINREQUIREDRATING_HACKBANKSERVER)) &&
								 (!isgov  || (isgov  && uplinkrating >= MINREQUIREDRATING_HACKGOVERNMENTCOMPUTER )) )
								return comp->logbank.TraceLog ( logbank_ip, comp->ip, date, uplinkrating );

						}

					}

				}

			}

		}
	}

	//
	// That log doesn't exist, or it can't be traced further
	// (this is the end of the trail).
	// If this is someone's personal computer, then he is guilty
	//

	if ( comp_local->TYPE == COMPUTER_TYPE_PERSONALCOMPUTER )
		return logbank_ip;

	else
		return NULL;

}

void LogBank::Empty ()
{

    DeleteDArrayData ( (DArray <UplinkObject *> *) &logs );
    DeleteDArrayData ( (DArray <UplinkObject *> *) &internallogs );

    logs.Empty ();
    internallogs.Empty ();

}

bool LogBank::Load ( FILE *file )
{

	LoadID ( file );

	int size;
	if ( !FileReadData ( &size, sizeof(size), 1, file ) ) return false;

    if ( size < 0 || size > MAX_ITEMS_DATA_STRUCTURE ) {
		UplinkPrintAbortArgs ( "WARNING: LogBank::Load, number of items appears to be wrong, size=%d", size );
		return false;
    }

	logs.SetSize ( size );
	internallogs.SetSize ( size );

	for ( int i = 0; i < size; ++i ) {

		// Load the log at this index

		int index;
		if ( !FileReadData ( &index, sizeof(index), 1, file ) ) return false;

		if ( index == -1 ) {

			// Empty space

		}
		else if ( index < 0 || index >= MAX_ITEMS_DATA_STRUCTURE ) {

			UplinkPrintAbortArgs ( "WARNING: LogBank::Load, number of items appears to be wrong, index=%d", index );
			return false;

		}
		else {

			AccessLog *al = new AccessLog ();
			if ( !al->Load ( file ) ) {
				delete al;
				return false;
			}
			logs.PutData ( al, index );

		}

		// If the internal log is different, load it now

		bool modified;
		if ( !FileReadData ( &modified, sizeof(modified), 1, file ) ) return false;

		if ( modified ) {

			AccessLog *al = new AccessLog ();
			if ( !al->Load ( file ) ) {
				delete al;
				return false;
			}
			internallogs.PutData ( al, index );

		}
		else {

            if ( index != -1 ) {

			    AccessLog *al = new AccessLog ();
				if ( al ) {
					if ( !logs.ValidIndex ( index ) ) {
						UplinkPrintAssert ( logs.ValidIndex ( index ) );
						delete al;
						return false;
					}
					al->SetProperties ( logs.GetData (index) );
				}
			    internallogs.PutData ( al, index );

            }

		}

	}

	LoadID_END ( file );

	return true;

}

void LogBank::Save ( FILE *file )
{

	SaveID ( file );

	/*
		Don't simply save each array - we can save serious space
		by remembering that logs and internallogs are usually identical - only a
		small number of logs are changed by hackers.
		*/

	int size = logs.Size ();
	fwrite ( &size, sizeof(size), 1, file );

	for ( int i = 0; i < size; ++i ) {

		// Save this log

		if ( logs.ValidIndex (i) ) {

			fwrite ( &i, sizeof(i), 1, file );
			logs.GetData (i)->Save ( file );

		}
		else {

			int dummy = -1;
			fwrite ( &dummy, sizeof(dummy), 1, file );

		}

		// If the internal log is different, save it as well

		bool modified = LogModified (i);
		fwrite ( &modified, sizeof(modified), 1, file );

		if ( modified ) {

			UplinkAssert ( internallogs.ValidIndex (i) );
			internallogs.GetData (i)->Save ( file );

		}

	}

	SaveID_END ( file );

}

void LogBank::Print ()
{

	printf ( "LogBank : \n" );
	PrintDArray ( (DArray <UplinkObject *> *) &logs );
	PrintDArray ( (DArray <UplinkObject *> *) &internallogs );

}

void LogBank::Update ()
{
}

char *LogBank::GetID ()
{

	return "LOGBANK";

}

int LogBank::GetOBJECTID ()
{

	return OID_LOGBANK;

}

// ============================================================================



AccessLog::AccessLog()
{

	TYPE = LOG_TYPE_NONE;

	UplinkStrncpy ( fromip, " ", sizeof ( fromip ) );

	SUSPICIOUS = false;

	data1 = data2 = data3 = NULL;

}

AccessLog::~AccessLog()
{

	if ( data1 ) delete [] data1;
	if ( data2 ) delete [] data2;
	if ( data3 ) delete [] data3;

}

void AccessLog::SetProperties ( Date *newdate, char *newfromip, char *newfromname,
							    int newSUSPICIOUS, int newTYPE )
{

	UplinkAssert (newdate);
	date.SetDate ( newdate );

	SetTYPE ( newTYPE );
	SetFromIP ( newfromip );

	UplinkStrncpy( fromname, newfromname, sizeof ( fromname ) );

	SetSuspicious ( newSUSPICIOUS );

}

void AccessLog::SetProperties ( AccessLog *copyme )
{

	UplinkAssert (copyme);

	SetProperties ( &(copyme->date), copyme->fromip, copyme->fromname, copyme->SUSPICIOUS, copyme->TYPE );
	SetData1 ( copyme->data1 );
	SetData2 ( copyme->data2 );
	SetData3 ( copyme->data3 );

}

void AccessLog::SetTYPE ( int newTYPE )
{

	TYPE = newTYPE;

}

void AccessLog::SetFromIP ( char *newfromip )
{

	UplinkAssert (strlen(newfromip) < SIZE_VLOCATION_IP );
	UplinkStrncpy ( fromip, newfromip, sizeof ( fromip ) );

}

void AccessLog::SetSuspicious ( int newSUSPICIOUS )
{

	SUSPICIOUS = newSUSPICIOUS;

}

void AccessLog::SetData1 ( char *newdata )
{

	if ( data1 ) delete [] data1;
	data1 = NULL;

	if ( newdata ) {
		data1 = new char [strlen(newdata)+1];
		UplinkSafeStrcpy ( data1, newdata );
	}

}

void AccessLog::SetData2 ( char *newdata )
{

	if ( data2 ) delete [] data2;
	data2 = NULL;

	if ( newdata ) {
		data2 = new char [strlen(newdata)+1];
		UplinkSafeStrcpy ( data2, newdata );
	}

}

void AccessLog::SetData3 ( char *newdata )
{

	if ( data3 ) delete [] data3;
	data3 = NULL;

	if ( newdata ) {
		data3 = new char [strlen(newdata)+1];
		UplinkSafeStrcpy ( data3, newdata );
	}

}

char *AccessLog::GetDescription ()
{

	std::ostrstream fullcaption;

    /*
        It's worth remembering that all logs can now
        be edited by a gamer, using the LogModifier tool.

        So it is not safe to assume that any data or fields
        exist, nor that their format will be correct.

        */

	switch ( TYPE ) {

		case LOG_TYPE_DELETED:
			fullcaption << "[Log Deleted]" << '\x0';
			break;

		case LOG_TYPE_CONNECTIONOPENED:
			fullcaption << "Connection established from " << fromip << '\x0';
			break;

		case LOG_TYPE_CONNECTIONCLOSED:
			fullcaption << "Connection from " << fromip << " closed" << '\x0';
			break;

		case LOG_TYPE_BOUNCEBEGIN:
			UplinkAssert (data1);
			fullcaption << "Connection opened to " << data1 << '\x0';
			break;

		case LOG_TYPE_BOUNCE:
			UplinkAssert (data1);
			fullcaption << "Connection from " << fromip << " routed to " << data1 << '\x0';
			break;

		case LOG_TYPE_TRANSFERTO:
		{

            if ( data1 && data2 ) {

			    char ip [SIZE_VLOCATION_IP];
			    char accno [16];
			    sscanf ( data1, "%s %s", ip, accno );

			    fullcaption << data2 << "c transfered to IP " << ip << ", acc no. " << accno << '\x0';

            }
            else {

                fullcaption << "Invalid TransferTO log" << '\x0';

            }

			break;

		}

		case LOG_TYPE_TRANSFERFROM:
		{

            if ( data1 && data2 ) {

			    char ip [SIZE_VLOCATION_IP];
			    char accno [16];
			    sscanf ( data1, "%s %s", ip, accno );

			    fullcaption << data2 << "c deposited from IP " << ip << ", acc no. " << accno << '\x0';

            }
            else {

                fullcaption << "Invalid TransferFROM log" << '\x0';

            }

			break;

		}

		default:
			UplinkAssert (data1);
			fullcaption << "From " << fromip << " : " << data1 << '\x0';
			break;

	};

	//return fullcaption.str ();
	char * ans = fullcaption.str();
    char * anscpy = new char[ strlen(ans) + 1 ];
	UplinkSafeStrcpy( anscpy, ans );
	ans = NULL;
	fullcaption.rdbuf()->freeze( 0 );
	return anscpy;

}

bool AccessLog::Load ( FILE *file )
{

	LoadID ( file );

	if ( !date.Load ( file ) ) return false;

	if ( !LoadDynamicStringStatic ( fromip, SIZE_VLOCATION_IP, file ) ) return false;
	if ( !LoadDynamicStringStatic ( fromname, SIZE_PERSON_NAME, file ) ) return false;

	if ( !FileReadData ( &TYPE, sizeof(TYPE), 1, file ) ) return false;
	if ( !FileReadData ( &SUSPICIOUS, sizeof(SUSPICIOUS), 1, file ) ) return false;

	if ( !LoadDynamicStringPtr ( &data1, file ) ) return false;
	if ( !LoadDynamicStringPtr ( &data2, file ) ) return false;
	if ( !LoadDynamicStringPtr ( &data3, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void AccessLog::Save ( FILE *file )
{

	SaveID ( file );

	date.Save ( file );

	SaveDynamicString ( fromip, SIZE_VLOCATION_IP, file );
	SaveDynamicString ( fromname, SIZE_PERSON_NAME, file );

	fwrite ( &TYPE, sizeof(TYPE), 1, file );
	fwrite ( &SUSPICIOUS, sizeof(SUSPICIOUS), 1, file );

	SaveDynamicString ( data1, file );
	SaveDynamicString ( data2, file );
	SaveDynamicString ( data3, file );

	SaveID_END ( file );

}

void AccessLog::Print ()
{

	printf ( "AccessLog : TYPE=%d\n", TYPE );
	date.Print ();
	printf ( "fromIP:%s, fromNAME:%s\n", fromip, fromname );

	if ( SUSPICIOUS == LOG_SUSPICIOUS )			printf ( "This was a suspicious action\n" );
	else if ( SUSPICIOUS ==
			  LOG_SUSPICIOUSANDNOTICED )		printf ( "This was suspicious and was noticed\n" );
	else if ( SUSPICIOUS ==
			  LOG_UNDERINVESTIGATION )			printf ( "This is under investigation\n" );
	else
												printf ( "This was not suspicious\n" );

	if ( data1 ) printf ( "Data1 : %s\n", data1 );
	if ( data2 ) printf ( "Data2 : %s\n", data2 );
	if ( data3 ) printf ( "Data3 : %s\n", data3 );

}

char *AccessLog::GetID ()
{

	return "ACC_LOG";

}

int AccessLog::GetOBJECTID ()
{

	return OID_ACCESSLOG;

}

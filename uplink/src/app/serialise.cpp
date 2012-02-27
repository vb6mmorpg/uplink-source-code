
#include "stdafx.h"

#include "gucci.h"

#include "app/app.h"
#include "app/serialise.h"
#include "app/globals.h"
#include "options/options.h"

#include "world/vlocation.h"
#include "world/person.h"
#include "world/message.h"
#include "world/agent.h"
#include "world/player.h"

#include "world/computer/computer.h"
#include "world/computer/logbank.h"
#include "world/computer/databank.h"
#include "world/computer/recordbank.h"
#include "world/computer/bankcomputer.h"
#include "world/computer/bankaccount.h"
#include "world/computer/securitysystem.h"
#include "world/computer/lancomputer.h"

#include "world/computer/computerscreen/genericscreen.h"
#include "world/computer/computerscreen/messagescreen.h"
#include "world/computer/computerscreen/passwordscreen.h"
#include "world/computer/computerscreen/menuscreen.h"
#include "world/computer/computerscreen/dialogscreen.h"
#include "world/computer/computerscreen/useridscreen.h"
#include "world/computer/computerscreen/logscreen.h"
#include "world/computer/computerscreen/bbsscreen.h"
#include "world/computer/computerscreen/linksscreen.h"
#include "world/computer/computerscreen/shareslistscreen.h"
#include "world/computer/computerscreen/cypherscreen.h"
#include "world/computer/computerscreen/highsecurityscreen.h"
#include "world/computer/computerscreen/disconnectedscreen.h"

#include "world/company/company.h"
#include "world/company/companyuplink.h"
#include "world/company/mission.h"
#include "world/company/sale.h"
#include "world/company/news.h"

#include "world/scheduler/notificationevent.h"
#include "world/scheduler/arrestevent.h"
#include "world/scheduler/shotbyfedsevent.h"
#include "world/scheduler/seizegatewayevent.h"
#include "world/scheduler/installhardwareevent.h"
#include "world/scheduler/warningevent.h"
#include "world/scheduler/attemptmissionevent.h"
#include "world/scheduler/runplotsceneevent.h"
#include "world/scheduler/changegatewayevent.h"
#include "world/scheduler/bankrobberyevent.h"

#include "mmgr.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))


void SaveBTree  ( BTree <UplinkObject *> *btree, FILE *file )
{

	UplinkAssert ( btree );

	DArray <UplinkObject *> *uo = btree->ConvertToDArray ();
	DArray <char *> *uo_id = btree->ConvertIndexToDArray ();

	int size = uo->Size ();

	int nbitem = 0;
	for ( int i = 0; i < size; ++i )
		if ( uo->ValidIndex (i) )
			nbitem++;

	if ( nbitem > MAX_ITEMS_DATA_STRUCTURE ) {
		UplinkPrintAbortArgs ( "WARNING: SaveBTree, number of items appears to be too big, size=%d, maxsize=%d",
		                       nbitem, MAX_ITEMS_DATA_STRUCTURE );
		nbitem = MAX_ITEMS_DATA_STRUCTURE;
	}

	fwrite ( &nbitem, sizeof(nbitem), 1, file );

	nbitem = 0;
	for ( int i = 0; i < size && nbitem < MAX_ITEMS_DATA_STRUCTURE; ++i ) {
		if ( uo->ValidIndex (i) ) {

			UplinkAssert ( uo_id->ValidIndex (i) );
			UplinkAssert ( uo->GetData (i) );

			SaveDynamicString ( uo_id->GetData (i), file );

			int OBJECTID = uo->GetData (i)->GetOBJECTID ();
			UplinkAssert ( OBJECTID != 0 );
			fwrite ( &OBJECTID, sizeof(int), 1, file );			

			uo->GetData (i)->Save ( file );

			nbitem++;

		}
	}

	delete uo;
	delete uo_id;

}

bool LoadBTree  ( BTree <UplinkObject *> *btree, FILE *file )
{

	if ( !btree ) {
		UplinkPrintAssert ( btree );
		return false;
	}

	int size;
	if ( !FileReadData ( &size, sizeof(size), 1, file ) ) return false;

    if ( size < 0 || size > MAX_ITEMS_DATA_STRUCTURE ) {
		UplinkPrintAbortArgs ( "WARNING: LoadBTree, number of items appears to be wrong, size=%d", size );
		return false;
    }

	for ( int i = 0; i < size; ++i ) {

		char *id = NULL;
		if ( !LoadDynamicStringPtr ( &id, file ) ) return false;
		if ( !id ) {
			UplinkPrintAbort ( "WARNING: LoadBTree NULL id" );
			return false;
		}

		int OBJECTID;
		if ( !FileReadData ( &OBJECTID, sizeof(int), 1, file ) ) return false;

		UplinkObject *uo = CreateUplinkObject ( OBJECTID );
		if ( !uo || !uo->Load ( file ) ) {
			delete [] id;
			if ( uo ) delete uo;
			return false;
		}

		btree->PutData ( id, uo );

		delete [] id;

	}

	return true;

}

void PrintBTree ( BTree <UplinkObject *> *btree )
{

	UplinkAssert ( btree );

	DArray <UplinkObject *> *uo = btree->ConvertToDArray ();
	DArray <char *> *uo_id = btree->ConvertIndexToDArray ();

	for ( int i = 0; i < uo->Size (); ++i ) {
		if ( uo->ValidIndex (i) ) {
			
			UplinkAssert ( uo_id->ValidIndex (i) );
			printf ( "Index = %s\n", uo_id->GetData (i) );

			if ( uo->GetData (i) )
				uo->GetData (i)->Print ();

			else
				printf ( "NULL\n" );

		}
	}

	delete uo;
	delete uo_id;

}

void UpdateBTree ( BTree <UplinkObject *> *btree )
{

	UplinkAssert ( btree );

	DArray <UplinkObject *> *uo = btree->ConvertToDArray ();

	for ( int i = 0; i < uo->Size (); ++i )
		if ( uo->ValidIndex (i) )
			if ( uo->GetData (i) )
				uo->GetData (i)->Update ();

	delete uo;	


}

void DeleteBTreeData ( BTree <UplinkObject *> *btree )
{

	UplinkAssert ( btree );

	DArray <UplinkObject *> *uo = btree->ConvertToDArray ();

	for ( int i = 0; i < uo->Size (); ++i )
		if ( uo->ValidIndex (i) )
			if ( uo->GetData (i) )				
				delete uo->GetData (i);

	delete uo;	

}

void SaveBTree ( BTree <char *> *btree, FILE *file )
{

	UplinkAssert ( btree );

	DArray <char *> *uo = btree->ConvertToDArray ();
	DArray <char *> *uo_id = btree->ConvertIndexToDArray ();

	int size = uo->Size ();

	int nbitem = 0;
	for ( int i = 0; i < size; ++i )
		if ( uo->ValidIndex (i) )
			nbitem++;

	if ( nbitem > MAX_ITEMS_DATA_STRUCTURE ) {
		UplinkPrintAbortArgs ( "WARNING: SaveBTree, number of items appears to be too big, size=%d, maxsize=%d",
		                       nbitem, MAX_ITEMS_DATA_STRUCTURE );
		nbitem = MAX_ITEMS_DATA_STRUCTURE;
	}

	fwrite ( &nbitem, sizeof(nbitem), 1, file );

	nbitem = 0;
	for ( int i = 0; i < size && nbitem < MAX_ITEMS_DATA_STRUCTURE; ++i ) {
		if ( uo->ValidIndex (i) ) {

			UplinkAssert ( uo_id->ValidIndex (i) );

			SaveDynamicString ( uo_id->GetData (i), file );
			SaveDynamicString ( uo->GetData (i), file );

			nbitem++;

		}
	}

	delete uo;
	delete uo_id;

}

bool LoadBTree ( BTree <char *> *btree, FILE *file )
{

	if ( !btree ) {
		UplinkPrintAssert ( btree );
		return false;
	}

	int size;
	if ( !FileReadData ( &size, sizeof(size), 1, file ) ) return false;

    if ( size < 0 || size > MAX_ITEMS_DATA_STRUCTURE ) {
		UplinkPrintAbortArgs ( "WARNING: LoadBTree, number of items appears to be wrong, size=%d", size );
		return false;
    }

	for ( int i = 0; i < size; ++i ) {

		char *id = NULL;
		if ( !LoadDynamicStringPtr ( &id, file ) ) return false;
		if ( !id ) {
			UplinkPrintAbort ( "WARNING: LoadBTree NULL id" );
			return false;
		}

		char *data = NULL;
		if ( !LoadDynamicStringPtr ( &data, file ) ) {
			delete [] id;
			return false;
		}

		btree->PutData ( id, data );
		delete [] id;

	}

	return true;

}

void PrintBTree	( BTree <char *> *btree )
{

	UplinkAssert ( btree );

	DArray <char *> *uo = btree->ConvertToDArray ();
	DArray <char *> *uo_id = btree->ConvertIndexToDArray ();

	for ( int i = 0; i < uo->Size (); ++i ) {
		if ( uo->ValidIndex (i) ) {
			
			UplinkAssert ( uo_id->ValidIndex (i) );
			printf ( "Index = %s\n", uo_id->GetData (i) );

			if ( uo->GetData (i) )
				printf ( "%s\n", uo->GetData (i) );

			else
				printf ( "NULL\n" );

		}
	}

	delete uo;
	delete uo_id;

}

void DeleteBTreeData ( BTree <char *> *btree )
{

	UplinkAssert ( btree );

	DArray <char *> *uo = btree->ConvertToDArray ();

	for ( int i = 0; i < uo->Size (); ++i )
		if ( uo->ValidIndex (i) )
			if ( uo->GetData (i) )
				// Even zero length string need to be deleted
				//if ( strcmp ( uo->GetData (i), "" ) != 0 )
					delete [] uo->GetData (i);

	delete uo;	

}


void SaveLList ( LList <UplinkObject *> *llist, FILE *file )
{

	UplinkAssert ( llist );

	int size = llist->Size ();
	int nbitem = 0;
	for ( int i = 0; i < size; ++i )
		if ( llist->GetData (i) )
			nbitem++;

	if ( nbitem > MAX_ITEMS_DATA_STRUCTURE ) {
		UplinkPrintAbortArgs ( "WARNING: SaveLList, number of items appears to be too big, size=%d, maxsize=%d",
		                       nbitem, MAX_ITEMS_DATA_STRUCTURE );
		nbitem = MAX_ITEMS_DATA_STRUCTURE;
	}

	fwrite ( &nbitem, sizeof(nbitem), 1, file );

	nbitem = 0;
	for ( int i = 0; i < size && nbitem < MAX_ITEMS_DATA_STRUCTURE; ++i ) {
		if ( llist->GetData (i) ) {
			int OBJECTID = llist->GetData (i)->GetOBJECTID ();
			UplinkAssert ( OBJECTID != 0 );
			fwrite ( &OBJECTID, sizeof(int), 1, file );
			llist->GetData (i)->Save ( file );
			nbitem++;
		}
	}

}

bool LoadLList ( LList <UplinkObject *> *llist, FILE *file )
{

	if ( !llist ) {
		UplinkPrintAssert ( llist );
		return false;
	}

	int size;
	if ( !FileReadData ( &size, sizeof(size), 1, file ) ) return false;

    if ( size < 0 || size > MAX_ITEMS_DATA_STRUCTURE ) {
		UplinkPrintAbortArgs ( "WARNING: LoadLList, number of items appears to be wrong, size=%d", size );
		return false;
    }

	for ( int i = 0; i < size; ++i ) {
	
		int OBJECTID;
		if ( !FileReadData ( &OBJECTID, sizeof(int), 1, file ) ) return false;
		UplinkObject *uo = CreateUplinkObject ( OBJECTID );

		if ( !uo || !uo->Load ( file ) ) {
			if ( uo ) delete uo;
			return false;
		}
		llist->PutData ( uo );

	}

	return true;

}

void PrintLList ( LList <UplinkObject *> *llist )
{

	UplinkAssert ( llist );

	for ( int i = 0; i < llist->Size (); ++i ) {

		printf ( "Index = %d\n", i );

		if ( llist->GetData (i) )
			llist->GetData (i)->Print ();

		else
			printf ( "NULL\n" );

	}

}

void UpdateLList ( LList <UplinkObject *> *llist )
{

	UplinkAssert ( llist );

	for ( int i = 0; i < llist->Size (); ++i )
		if ( llist->GetData (i) )
			llist->GetData (i)->Update ();

}

void DeleteLListData ( LList <UplinkObject *> *llist )
{

	UplinkAssert ( llist );

	for ( int i = 0; i < llist->Size (); ++i )
		if ( llist->GetData (i) )
			delete llist->GetData (i);

}

void SaveLList ( LList <char *> *llist, FILE *file )
{

	UplinkAssert ( llist );

	int size = llist->Size ();

	if ( size > MAX_ITEMS_DATA_STRUCTURE ) {
		UplinkPrintAbortArgs ( "WARNING: SaveLList, number of items appears to be too big, size=%d, maxsize=%d",
		                       size, MAX_ITEMS_DATA_STRUCTURE );
		size = MAX_ITEMS_DATA_STRUCTURE;
	}

	fwrite ( &size, sizeof(size), 1, file );

	for ( int i = 0; i < size; ++i ) 
		SaveDynamicString ( llist->GetData (i), file );

}

bool LoadLList ( LList <char *> *llist, FILE *file )
{

	if ( !llist ) {
		UplinkAssert ( llist );
		return false;
	}

	int size;
	if ( !FileReadData ( &size, sizeof(size), 1, file ) ) return false;

    if ( size < 0 || size > MAX_ITEMS_DATA_STRUCTURE ) {
		UplinkPrintAbortArgs ( "WARNING: LoadLList, number of items appears to be wrong, size=%d", size );
		return false;
    }

	for ( int i = 0; i < size; ++i ) {

		char *stringdata = NULL;
		if ( !LoadDynamicStringPtr ( &stringdata, file ) ) return false;
		llist->PutData ( stringdata );

	}

	return true;
			
}

void PrintLList	( LList <char *> *llist )
{
	
	UplinkAssert ( llist );

	for ( int i = 0; i < llist->Size (); ++i ) {
		if ( llist->GetData (i) )
			printf ( "Index = %d : %s\n", i, llist->GetData (i) );
		else
			printf ( "Index = %d : NULL\n", i );
	}

}

void DeleteLListData ( LList <char *> *llist )
{

	UplinkAssert ( llist );

	for ( int i = 0; i < llist->Size (); ++i ) 
		if ( llist->GetData (i) )
			// Even zero length string need to be deleted
			//if ( strlen(llist->GetData (i)) != 0 )
			//	if ( strcmp(llist->GetData (i), "") != 0 )
					delete [] llist->GetData (i);
					

}

void SaveDArray ( DArray <UplinkObject *> *darray, FILE *file )
{

	UplinkAssert ( darray );

	int size = darray->Size ();

	if ( size > MAX_ITEMS_DATA_STRUCTURE ) {
		UplinkPrintAbortArgs ( "WARNING: SaveDArray, number of items appears to be too big, size=%d, maxsize=%d",
		                       size, MAX_ITEMS_DATA_STRUCTURE );
		size = MAX_ITEMS_DATA_STRUCTURE;
	}

	fwrite ( &size, sizeof(size), 1, file );

	for ( int i = 0; i < size; ++i ) {
		if ( darray->ValidIndex (i) && darray->GetData (i) ) {

			fwrite ( &i, sizeof(i), 1, file );

			int OBJECTID = darray->GetData (i)->GetOBJECTID ();
			UplinkAssert ( OBJECTID != 0 );
			fwrite ( &OBJECTID, sizeof(int), 1, file );

			darray->GetData (i)->Save ( file );

		}
		else {

			int dummy = -1;
			fwrite ( &dummy, sizeof(dummy), 1, file );

		}
	}

}

bool LoadDArray ( DArray <UplinkObject *> *darray, FILE *file )
{

	if ( !darray ) {
		UplinkPrintAssert ( darray );
		return false;
	}

	int size;
	if ( !FileReadData ( &size, sizeof(size), 1, file ) ) return false;

    if ( size < 0 || size > MAX_ITEMS_DATA_STRUCTURE ) {
		UplinkPrintAbortArgs ( "WARNING: LoadDArray, number of items appears to be wrong, size=%d", size );
		return false;
    }

	darray->SetSize ( size );

	for ( int i = 0; i < size; ++i ) {

		int index;
		if ( !FileReadData ( &index, sizeof(index), 1, file ) ) return false;

		if ( index == -1 ) {

			// Empty space

		}
		else if ( index < 0 || index >= MAX_ITEMS_DATA_STRUCTURE ) {

			UplinkPrintAbortArgs ( "WARNING: LoadDArray, number of items appears to be wrong, index=%d", index );
			return false;

		}
		else {

			int OBJECTID;
			if ( !FileReadData ( &OBJECTID, sizeof(int), 1, file ) ) return false;
			if ( OBJECTID == OID_UNKNOWN ) {
				UplinkPrintAbort ( "LoadDArray WARNING : OID_UNKNOWN as OBJECTID" );
				return false;
			}

			UplinkObject *uo = CreateUplinkObject ( OBJECTID );
			if ( !uo || !uo->Load ( file ) ) {
				if ( uo ) delete uo;
				return false;
			}

			darray->PutData ( uo, index );			// Insert into the correct index

		}

	}

	return true;

}

void PrintDArray ( DArray <UplinkObject *> *darray )
{

	UplinkAssert ( darray );

	for ( int i = 0; i < darray->Size (); ++i ) {

		printf ( "Index = %d\n", i );

		if ( darray->ValidIndex (i) ) {

			if ( darray->GetData (i) )
				darray->GetData (i)->Print ();

			else
				printf ( "NULL\n" );

		}
		else {

			printf ( "Not a Valid Index\n" );

		}

	}
	
}

void UpdateDArray ( DArray <UplinkObject *> *darray )
{

	UplinkAssert ( darray );

	for ( int i = 0; i < darray->Size (); ++i )
		if ( darray->ValidIndex ( i ) )
			if ( darray->GetData (i) )
				darray->GetData (i)->Update ();

}

void DeleteDArrayDataD( DArray <UplinkObject *> *darray, const char * file, int line )
{

	UplinkAssert ( darray );

	for ( int i = 0; i < darray->Size (); ++i )
		if ( darray->ValidIndex ( i ) )
			if ( darray->GetData (i) ) {
				try {
					UplinkObject * upobj = darray->GetData (i);
					delete upobj;
					darray->RemoveData( i );
				} catch(...) {
					// Log the attempted deletion.
					DEBUG_PRINTF( "Oops! Tried to delete something that wasn't there [%s (%d)].\n",
						file, line );
				}
			}

}

void DeleteDArrayDataD( DArray <char *> *darray, const char * file, int line )
{

	UplinkAssert ( darray );

	for ( int i = 0; i < darray->Size (); ++i ) 
		if ( darray->ValidIndex (i) )
            if ( darray->GetData (i) )
				// Even zero length string need to be deleted
			    //if ( strlen(darray->GetData (i)) != 0 )
					try {
					    delete [] darray->GetData (i);
						darray->RemoveData( i );
					} catch(...) {
						DEBUG_PRINTF( "Oops! Tried to delete something that wasn't there [%s (%d)].\n",
							file, line );
					}

}

void SaveDArray ( DArray <int> *darray, FILE *file )
{

	UplinkAssert ( darray );

	int size = darray->Size ();

	if ( size > MAX_ITEMS_DATA_STRUCTURE ) {
		UplinkPrintAbortArgs ( "WARNING: SaveDArray, number of items appears to be too big, size=%d, maxsize=%d",
		                       size, MAX_ITEMS_DATA_STRUCTURE );
		size = MAX_ITEMS_DATA_STRUCTURE;
	}

	fwrite ( &size, sizeof(size), 1, file );

	for ( int i = 0; i < size; ++i ) {
		if ( darray->ValidIndex (i) ) {

			int data = darray->GetData (i);

			fwrite ( &i, sizeof(i), 1, file );
			fwrite ( &data, sizeof(data), 1, file );

		}
		else {

			int dummy = -1;
			fwrite ( &dummy, sizeof(dummy), 1, file );

		}
	}

}

bool LoadDArray ( DArray <int> *darray, FILE *file )
{

	if ( !darray ) {
		UplinkPrintAssert ( darray );
		return false;
	}

	int size;
	if ( !FileReadData ( &size, sizeof(size), 1, file ) ) return false;

    if ( size < 0 || size > MAX_ITEMS_DATA_STRUCTURE ) {
		UplinkPrintAbortArgs ( "WARNING: LoadDArray, number of items appears to be wrong, size=%d", size );
		return false;
    }

	darray->SetSize ( size );

	for ( int i = 0; i < size; ++i ) {

		int index;
		if ( !FileReadData ( &index, sizeof(index), 1, file ) ) return false;

		if ( index == -1 ) {

			// Empty space

		}
		else if ( index < 0 || index >= MAX_ITEMS_DATA_STRUCTURE ) {

			UplinkPrintAbortArgs ( "WARNING: LoadDArray, number of items appears to be wrong, index=%d", index );
			return false;

		}
		else {

			int data;
			if ( !FileReadData ( &data, sizeof(data), 1, file ) ) return false;

			darray->PutData ( data, index );			// Insert into the correct index

		}

	}

	return true;

}

void PrintDArray ( DArray <int> *darray )
{

	UplinkAssert ( darray );

	for ( int i = 0; i < darray->Size (); ++i ) {

		printf ( "Index %d : ", i );

		if ( darray->ValidIndex (i) )
			printf ( "%d\n", darray->GetData (i) );

		else
			printf ( "Not a valid index\n" );

	}

}

UplinkObject *CreateUplinkObject ( int OBJECTID )
{

	UplinkObject *uo = NULL;

	switch ( OBJECTID ) {
		
		case OID_VLOCATION				: uo = new VLocation ();			break;	
		case OID_MESSAGE				: uo = new Message ();				break;
		case OID_PERSON					: uo = new Person ();				break;
		case OID_AGENT					: uo = new Agent ();				break;
		case OID_PLAYER					: uo = new Player ();				break;
		case OID_OPTION					: uo = new Option ();				break;
        case OID_VLOCATIONSPECIAL       : uo = new VLocationSpecial ();     break;

		case OID_COMPUTER				: uo = new Computer ();				break;
		case OID_DATA					: uo = new Data ();					break;
		case OID_ACCESSLOG				: uo = new AccessLog ();			break;
		case OID_RECORD					: uo = new Record ();				break;
		case OID_BANKCOMPUTER			: uo = new BankComputer ();			break;
		case OID_BANKACCOUNT			: uo = new BankAccount ();			break;
		case OID_SECURITYSYSTEM			: uo = new SecuritySystem ();		break;
        case OID_LANCOMPUTER            : uo = new LanComputer ();          break;
        case OID_LANCOMPUTERSYSTEM      : uo = new LanComputerSystem ();    break;
        case OID_LANCOMPUTERLINK        : uo = new LanComputerLink ();      break;

		case OID_GENERICSCREEN			: uo = new GenericScreen ();		break;
		case OID_MESSAGESCREEN			: uo = new MessageScreen ();		break;
		case OID_PASSWORDSCREEN			: uo = new PasswordScreen ();		break;
		case OID_MENUSCREEN				: uo = new MenuScreen ();			break;
		case OID_MENUSCREENOPTION		: uo = new MenuScreenOption ();		break;
		case OID_DIALOGSCREEN			: uo = new DialogScreen ();			break;
		case OID_DIALOGSCREENWIDGET		: uo = new DialogScreenWidget ();	break;
		case OID_USERIDSCREEN			: uo = new UserIDScreen ();			break;
		case OID_LOGSCREEN				: uo = new LogScreen ();			break;
		case OID_BBSSCREEN				: uo = new BBSScreen ();			break;
		case OID_LINKSSCREEN			: uo = new LinksScreen ();			break;
		case OID_SHARESLISTSCREEN		: uo = new SharesListScreen ();		break;
		case OID_CYPHERSCREEN			: uo = new CypherScreen ();			break;
		case OID_HIGHSECURITYSCREEN		: uo = new HighSecurityScreen ();	break;
        case OID_DISCONNECTEDSCREEN     : uo = new DisconnectedScreen ();   break;
			
		case OID_COMPANY				: uo = new Company ();				break;
		case OID_COMPANYUPLINK			: uo = new CompanyUplink ();		break;
		case OID_MISSION				: uo = new Mission ();				break;
		case OID_SALE					: uo = new Sale ();					break;
		case OID_SALEVERSION			: uo = new SaleVersion ();			break;
		case OID_NEWS					: uo = new News ();					break;

		case OID_NOTIFICATIONEVENT		: uo = new NotificationEvent ();	break;
		case OID_ARRESTEVENT			: uo = new ArrestEvent ();			break;
		case OID_SHOTBYFEDSEVENT		: uo = new ShotByFedsEvent ();		break;
		case OID_SEIZEGATEWAYEVENT		: uo = new SeizeGatewayEvent ();	break;
		case OID_INSTALLHWEVENT			: uo = new InstallHardwareEvent ();	break;
		case OID_WARNINGEVENT			: uo = new WarningEvent ();			break;
		case OID_ATTEMPTMISSIONEVENT	: uo = new AttemptMissionEvent ();	break;
		case OID_RUNPLOTSCENEEVENT		: uo = new RunPlotSceneEvent ();	break;
		case OID_CHANGEGATEWAYEVENT		: uo = new ChangeGatewayEvent ();	break;
        case OID_BANKROBBERYEVENT       : uo = new BankRobberyEvent ();     break;			

		case OID_UNKNOWN				: UplinkPrintAbort ( "OBJECTID Not assigned" ); break;
		default							: UplinkPrintAbortArgs ( "Unrecognised OBJECTID=%d", OBJECTID );

	}

	return uo;

}

bool LoadDynamicStringInt ( char* _file, int _line, char **string, FILE *file )
{

	*string = NULL;

	int size;
	if ( !FileReadData ( &size, sizeof(size), 1, file ) ) return false;

	if ( size == -1 ) {

		// Nothing to do, NULL string.

	}
    else if ( size < 0 || size > MAX_LENGTH_DYMANIC_STRING ) {

		UplinkPrintAbortArgs ( "WARNING: LoadDynamicString, size appears to be wrong, size=%d, %s:%d", size, _file, _line );
		return false;

    }
	else {

		*string = new char [size+1];
		if ( !FileReadData ( *string, size, 1, file ) ) {
			(*string) [ size ] = '\0';
			return false;
		}
		(*string) [ size ] = '\0';
	
	}

	return true;

}

bool LoadDynamicStringInt ( char* _file, int _line, char *string, int maxsize, FILE *file )
{

	if ( !string ) {
		UplinkPrintAssert (string);
		return false;
	}

	if ( maxsize > 0 )
		string [ 0 ] = '\0';

	int size;
	if ( !FileReadData ( &size, sizeof(size), 1, file ) ) return false;

	if ( size == -1 ) {

        UplinkPrintAbortArgs ( "WARNING: LoadDynamicString, empty string, %s:%d", _file, _line );
		return false;

	}
	else if ( size > maxsize ) {

        UplinkPrintAbortArgs ( "WARNING: LoadDynamicString, size > maxsize, size=%d, maxsize=%d, %s:%d", size, maxsize, _file, _line );

		if ( maxsize > 0 ) {

			if ( !FileReadData ( string, maxsize, 1, file ) ) {
				string [ maxsize - 1 ] = '\0';
				return false;
			}
			string [ maxsize - 1 ] = '\0';

			UplinkPrintAbortArgs ( "WARNING: LoadDynamicString, possible string=%s, %s:%d", string, _file, _line );

		}

		return false;

	}
    else if ( size < 0 ) {

        UplinkPrintAbortArgs ( "WARNING: LoadDynamicString, size appears to be wrong, size=%d, %s:%d", size, _file, _line );
		return false;

    }
	else {
		
		if ( !FileReadData ( string, size, 1, file ) ) {
			string [ size - 1 ] = '\0';
			return false;
		}
		string [ size - 1 ] = '\0';
	
	}

	return true;

}

void SaveDynamicString ( char *string, FILE *file )
{
	SaveDynamicString ( string, -1, file );
}

void SaveDynamicString ( char *string, int maxsize, FILE *file )
{

	if ( string ) {

		int realmaxsize = MAX_LENGTH_DYMANIC_STRING;
		if ( maxsize > 0 )
			realmaxsize = min ( MAX_LENGTH_DYMANIC_STRING, maxsize );

		int size = (int) ( strlen(string) + 1 );
		if ( size <= realmaxsize ) {

			fwrite ( &size, sizeof(size), 1, file );
			if ( size > 1 )
				fwrite ( string, size - 1, 1, file );
			char terminatingchar = '\0';
			fwrite ( &terminatingchar, 1, 1, file );

		}
		else {

			UplinkPrintAbortArgs ( "WARNING: SaveDynamicString, size appears to be too long, size=%d, maxsize=%d, absolute maxsize=%d",
			                       size, maxsize, MAX_LENGTH_DYMANIC_STRING );

			fwrite ( &realmaxsize, sizeof(realmaxsize), 1, file );
			if ( realmaxsize > 1 )
				fwrite ( string, realmaxsize - 1, 1, file );
			char terminatingchar = '\0';
			fwrite ( &terminatingchar, 1, 1, file );

		}

	}
	else {

		int size = -1;
		fwrite ( &size, sizeof(size), 1, file );

	}

}

bool FileReadDataInt ( char* _file, int _line, void * _DstBuf, size_t _ElementSize, size_t _Count, FILE * _File )
{
	size_t sizeRead = fread ( _DstBuf, _ElementSize, _Count, _File );
	if ( sizeRead != _Count ) {
		UplinkPrintAbortArgs ( "WARNING: FileReadDataInt, request read count is different then the readed count, request=%d, readed=%d, errno=%d, %s:%d",
		                       _Count, sizeRead, errno, _file, _line );
		return false;
	}
	return true;
}

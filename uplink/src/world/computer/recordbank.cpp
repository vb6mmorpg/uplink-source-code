// computerrecord.cpp: implementation of the computerrecord class.
//
//////////////////////////////////////////////////////////////////////

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "world/computer/recordbank.h"
#include "world/generator/numbergenerator.h"

#include "mmgr.h"


RecordBank::RecordBank ()
{

}


RecordBank::~RecordBank ()
{

    DeleteLListData ( (LList <UplinkObject *> *) &records );

}

void RecordBank::AddRecord ( Record *newrecord )
{

	UplinkAssert (newrecord);
	records.PutData ( newrecord );

}

void RecordBank::AddRecordSorted ( Record *newrecord, char *sortfield )
{

	UplinkAssert (newrecord);
	UplinkAssert (sortfield);

	char *newvalue = newrecord->GetField ( sortfield );
	UplinkAssert (newvalue);

	bool inserted = false;

	for ( int i = 0; i < records.Size (); ++i ) {
		if ( records.GetData (i) ) {

			char *fieldvalue = records.GetData (i)->GetField ( sortfield );
			UplinkAssert (fieldvalue);

			if ( strcmp ( fieldvalue, newvalue ) > 0 ) {
				records.PutDataAtIndex ( newrecord, i );
				inserted = true;
				break;
			}

		}
	}
			
	if ( !inserted ) records.PutDataAtEnd ( newrecord );

}

char * RecordBank::MakeSafeField( char * fieldval )
{
	size_t len = strlen( fieldval );
	char * val = new char[ len + 1 ];
	UplinkSafeStrcpy( val, fieldval );
	for ( size_t i = 0; i < len; ++i )
		if ( val[i] == ';' ) val[i] = '.';
	return val;
}

Record *RecordBank::GetRecord ( int index )
{

	if ( records.ValidIndex (index) )
		return records [index];

	else
		return NULL;

}

Record *RecordBank::GetRecord ( char *query )
{

	LList <Record *> *result = GetRecords ( query );

    if ( !result ) {

		return NULL;

	}
    else if ( result->Size () == 0 ) {

        delete result;
        return NULL;

    }
	else if ( result->Size () > 1 ) {

		printf ( "RecordBank::GetRecord, found more than 1 match, returning the first\n" );
		UplinkAssert ( result->ValidIndex (0) );
		Record *record = result->GetData (0);
        delete result;
        return record;

	}
	else {

		UplinkAssert ( result->ValidIndex (0) );
        Record *record = result->GetData (0);
        delete result;
		return record;

	}

}

Record *RecordBank::GetRecordFromName( char *name )
{
    char query[256];
	char * tempname = MakeSafeField( name );

	UplinkAssert ( ( sizeof ( "%s = %s" ) + sizeof ( RECORDBANK_NAME ) + strlen ( tempname ) ) < sizeof ( query ) );

    UplinkSnprintf( query, sizeof ( query ), "%s = %s", RECORDBANK_NAME, tempname );
	delete [] tempname;
    return GetRecord( query );
}

Record *RecordBank::GetRecordFromNamePassword ( char *name, char *password )
{
    char query[256];
	char * tempname = MakeSafeField( name );
	char * passwd = MakeSafeField( password );

	UplinkAssert ( ( sizeof ( "%s = %s ; %s = %s" ) + sizeof ( RECORDBANK_NAME ) + strlen ( tempname ) + 
	                 sizeof ( RECORDBANK_PASSWORD ) + strlen ( passwd ) ) < sizeof ( query ) );

	UplinkSnprintf( query, sizeof ( query ), "%s = %s ; %s = %s", RECORDBANK_NAME, tempname, 
																	 RECORDBANK_PASSWORD, passwd );
	delete [] tempname;
	delete [] passwd;
    return GetRecord( query );
}

Record *RecordBank::GetRecordFromAccountNumber ( char *accNo )
{
    char query[256];
	char * tempAccNo = MakeSafeField( accNo );

	UplinkAssert ( ( sizeof ( "%s = %s" ) + sizeof ( RECORDBANK_ACCNO ) + strlen ( tempAccNo ) ) < sizeof ( query ) );

	UplinkSnprintf( query, sizeof ( query ), "%s = %s", RECORDBANK_ACCNO, tempAccNo );
	delete [] tempAccNo;
    return GetRecord( query );
}

LList <Record *> *RecordBank::GetRecords ( char *query )
{

	// Make a copy of the query
	
	char *localquery = new char [strlen(query)+1];
	UplinkSafeStrcpy ( localquery, query );

	// Calculate the number of conditions 

	int numconditions = 1;
	for ( char *p = localquery; *p != 0; ++p )
		if ( *p == ';' )
			++numconditions;

	// Create our data structures to represent the conditions

	char **condition = new char *[numconditions];
	condition [0] = localquery;
	int i;

	for ( i = 1; i < numconditions; ++i ) {
		condition [i] = strchr ( condition [i-1], ';' );		
		UplinkAssert ( *(condition [i]-1) == ' ' );			// Check the ';' is surrounded by spaces
		UplinkAssert ( *(condition [i]+1) == ' ' );
		*(condition [i] - 1) = '\x0';						// Replace the space before the ';' with a '\x0'
		(condition [i]) += 2;								// Point condition at char after '\x0'
	}

	// Parse each condition into 3 pieces of data - fieldname, operation and required value
	
	char **fields	= new char *[numconditions];
	char *ops		= new char  [numconditions];
	char **values	= new char *[numconditions];

	for ( i = 0; i < numconditions; ++i ) {

		fields [i] = condition [i];

		char *oplocation;

		if      ( strchr ( condition [i], '=' )	)	oplocation = strchr ( condition [i], '=' );
		else if ( strchr ( condition [i], '!' )	)	oplocation = strchr ( condition [i], '!' );
		else if ( strchr ( condition [i], '+' )	)	oplocation = strchr ( condition [i], '+' );
		else if ( strchr ( condition [i], '-' )	)	oplocation = strchr ( condition [i], '-' );
		else	UplinkAbort ( "RecordBank::GetRecords, invalid query" );
		
		UplinkAssert ( *(oplocation - 1) == ' ' );	// Check the op is surrounded by spaces
		UplinkAssert ( *(oplocation + 1) == ' ' );

		*(oplocation - 1) = '\x0';					// Terminate the field string before the op
		
		ops [i] = *(oplocation);					// Get the operation character
		values [i] = oplocation + 2;				// Get the required value

	}

	// Test the conditions on each record 
	// Add any successes into the results list
	
	LList <Record *> *results = new LList <Record *> ();
	
	for ( int ri = 0; ri < records.Size (); ++ri ) {
		if ( records.ValidIndex (ri) ) {

			Record *rec = records.GetData (ri);
			int nummatches = 0;

			for ( i = 0; i < numconditions; ++i ) {

				char *reqfield = fields [i];							// Required field name
				char *reqvalue = values [i];							// Required field value
				char *thisvalue = rec->GetField ( reqfield );			// Actual field value
				
				if ( thisvalue ) {

					switch ( ops [i] ) {
						case '=':	if ( strcmp ( thisvalue, reqvalue ) == 0 ) ++nummatches;		break;
						case '!':   if ( strcmp ( thisvalue, reqvalue ) != 0 ) ++nummatches;		break;
						case '+':	if ( strstr ( thisvalue, reqvalue ) )	   ++nummatches;		break;
						case '-':	if ( !strstr ( thisvalue, reqvalue ) )	   ++nummatches;		break;
						default:	UplinkAbort ( "RecordBank::GetRecord, unrecognised op code" );
					}

				}

			}

			if ( nummatches == numconditions )
				results->PutData ( rec );

		}
	}

    // Clean up used memory

    delete [] localquery;
    delete [] condition;
    delete [] fields;
    delete [] ops;
    delete [] values;

	// Return the results

	if ( results->Size () > 0 ) 
		return results;

    else {
        delete results;
		return NULL;
    }

}

Record *RecordBank::GetRandomRecord ( char *query )
{

	LList <Record *> *records = GetRecords ( query );

	if ( records ) {

		int index = NumberGenerator::RandomNumber ( records->Size () );
		UplinkAssert ( records->ValidIndex (index) );
        Record *result = records->GetData (index);
        delete records;
        return result;

	}
    else 
		return NULL;

}

bool RecordBank::Load ( FILE *file )
{

	LoadID ( file );

	if ( !LoadLList ( (LList <UplinkObject *> *) &records, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void RecordBank::Save ( FILE *file )
{

	SaveID ( file );

	SaveLList ( (LList <UplinkObject *> *) &records, file );

	SaveID_END ( file );

}

void RecordBank::Print ()
{

	printf ( "RecordBank\n" );
	PrintLList ( (LList <UplinkObject *> *) &records );

}

void RecordBank::Update ()
{

}
	
char *RecordBank::GetID ()
{

	return "RECBANK";

}

int RecordBank::GetOBJECTID ()
{

	return OID_RECORDBANK;

}

//////////////////////////////////////////////////////////////////////

Record::Record()
{

}

Record::~Record()
{

    DeleteBTreeData ( &fields );

}

void Record::AddField ( char *name, char *value )
{

	char *newvalue = new char [strlen(value) + 1];
	UplinkSafeStrcpy ( newvalue, value );
	fields.PutData ( name, newvalue );

}

void Record::AddField ( char *name, int value )
{

	size_t newvaluesize = 16;
	char *newvalue = new char [newvaluesize];
	UplinkSnprintf ( newvalue, newvaluesize, "%d", value );
	fields.PutData ( name, newvalue );

}

void Record::ChangeField ( char *name, char *newvalue )
{

	BTree <char *> *tree = fields.LookupTree ( name );

	if ( tree ) {

		if ( tree->data ) delete [] tree->data;
		size_t tree__datasize = strlen(newvalue) + 1;
		tree->data = new char [tree__datasize];
		UplinkStrncpy ( tree->data, newvalue, tree__datasize );

	}
	else {

		printf ( "Record::ChangeField, WARNING : field %s not found (created instead)\n", name );
		AddField ( name, newvalue );

	}

}

void Record::ChangeField ( char *name, int newvalue )
{

	BTree <char *> *tree = fields.LookupTree ( name );

	if ( tree ) {

		if ( tree->data ) delete [] tree->data;
		size_t tree__datasize = 8;
		tree->data = new char [tree__datasize];
		UplinkSnprintf ( tree->data, tree__datasize, "%d", newvalue );

	}
	else {

		printf ( "Record::ChangeField, WARNING : field %s not found (created instead)\n", name );
		AddField ( name, newvalue );

	}

}

char *Record::GetField ( char *name )
{

	if ( !name ) return NULL;

	BTree <char *> *tree = fields.LookupTree ( name );

	if ( tree ) {

		return tree->data;

	}
	else {

		return NULL;

	}

}

void Record::DeleteField ( char *name )
{

	fields.RemoveData ( name );

}

int RecordBank::FindNextRecordIndexNameNotSystemAccount ( int curindex )
{

	int recordindex = 0;
	if ( curindex != -1 )
		recordindex = curindex + 1;

	while ( recordindex < records.Size () ) {

		Record *rec = GetRecord ( recordindex );
		UplinkAssert (rec);
		char *thisname = rec->GetField ( RECORDBANK_NAME );
		UplinkAssert (thisname);

		if ( strcmp ( thisname, RECORDBANK_ADMIN ) != 0 && 
		     strcmp ( thisname, RECORDBANK_READWRITE ) != 0 &&
			 strcmp ( thisname, RECORDBANK_READONLY ) != 0 ) {

			return recordindex;
		}

		recordindex++;

	}

	return -1;

}

bool Record::Load  ( FILE *file )
{

	LoadID ( file );

	if ( !LoadBTree ( &fields, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void Record::Save  ( FILE *file )
{

	SaveID ( file );

	SaveBTree ( &fields, file );

	SaveID_END ( file );

}

void Record::Print ()
{

	printf ( "Record :\n" );

	DArray <char *> *field_names = fields.ConvertIndexToDArray ();
	DArray <char *> *field_values = fields.ConvertToDArray ();

	for ( int i = 0; i < field_names->Size (); ++i ) {

		UplinkAssert ( field_names->ValidIndex (i) );
		UplinkAssert ( field_values->ValidIndex (i) );

		printf ( "%s : %s\n", field_names->GetData (i), field_values->GetData (i) );

	}

	delete field_names;
	delete field_values;

}

void Record::Update ()
{
}
	
char *Record::GetID ()
{

	return "RECORD";

}

int Record::GetOBJECTID ()
{

	return OID_RECORD;

}


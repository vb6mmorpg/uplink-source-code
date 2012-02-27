
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "world/company/news.h"

#include "mmgr.h"


News::News ()
{

	details = NULL;
	NEWSTYPE = NEWS_TYPE_NONE;

    UplinkStrncpy ( headline, " ", sizeof ( headline ) );
    UplinkStrncpy ( data1, " ", sizeof ( data1 ) );
    UplinkStrncpy ( data2, " ", sizeof ( data2 ) );

}

News::~News ()
{

	if ( details ) delete [] details;
		
}

void News::SetDate ( Date *newdate )
{

	date.SetDate ( newdate );

}

void News::SetHeadline ( char *newheadline )
{

	UplinkAssert ( strlen ( newheadline ) < SIZE_NEWS_HEADLINE );
	UplinkStrncpy ( headline, newheadline, sizeof ( headline ) );

}

void News::SetDetails ( char *newdetails )
{

	if ( details )
		delete [] details;
	details = new char [strlen(newdetails)+1];
	UplinkSafeStrcpy ( details, newdetails );

}

char *News::GetDetails ()
{

	return details;

}

void News::SetData ( int newNEWSTYPE, char *newdata1, char *newdata2 )
{

	NEWSTYPE = newNEWSTYPE;

	if ( newdata1 ) {
		UplinkAssert (strlen(newdata1) < SIZE_NEWS_DATA);
		UplinkStrncpy ( data1, newdata1, sizeof ( data1 ) );
	}
	else
		UplinkStrncpy ( data1, " ", sizeof ( data1 ) );

	if ( newdata2 ) {
		UplinkAssert (strlen(newdata2) < SIZE_NEWS_DATA);
		UplinkStrncpy ( data2, newdata2, sizeof ( data2 ) );
	}
	else
		UplinkStrncpy ( data2, " ", sizeof ( data2 ) );

}

bool News::Load  ( FILE *file )
{
	
	LoadID ( file );

	if ( !date.Load ( file ) ) return false;
	if ( !FileReadData ( &NEWSTYPE,	sizeof(NEWSTYPE), 1, file ) ) return false;

	if ( !LoadDynamicStringStatic ( headline, SIZE_NEWS_HEADLINE, file ) ) return false;
	if ( !LoadDynamicStringStatic ( data1, SIZE_NEWS_DATA, file ) ) return false;
	if ( !LoadDynamicStringStatic ( data2, SIZE_NEWS_DATA, file ) ) return false;

	if ( !LoadDynamicStringPtr ( &details, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void News::Save  ( FILE *file )
{
	
	SaveID ( file );

	date.Save ( file );
	fwrite ( &NEWSTYPE, sizeof(NEWSTYPE), 1, file );

	SaveDynamicString ( headline, SIZE_NEWS_HEADLINE, file );
	SaveDynamicString ( data1, SIZE_NEWS_DATA, file );
	SaveDynamicString ( data2, SIZE_NEWS_DATA, file );

	SaveDynamicString ( details, file );

	SaveID_END ( file );

}

void News::Print ()
{

	printf ( "News: %s\n", headline );
	date.Print ();
	printf ( "%s\n", details );
	
	printf ( "NEWSTYPE:%d\n", NEWSTYPE );
	printf ( "data1 = %s\n", data1 );
	printf ( "data2 = %s\n", data2 );
		
}
	
char *News::GetID ()
{

	return "NEWS";

}

int News::GetOBJECTID ()
{

	return OID_NEWS;

}

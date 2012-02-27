
/*

  News object

	Represents a single news story, posted onto 
	one of the news servers

  */


#ifndef _included_news_h
#define _included_news_h

// ============================================================================

#include "app/uplinkobject.h"
#include "world/date.h"

#define SIZE_NEWS_HEADLINE	128
#define SIZE_NEWS_DATA		128

#define NEWS_TYPE_NONE		0
#define NEWS_TYPE_HACKED	1				// Data1 = IP
#define NEWS_TYPE_ARREST	2				// Data1 = name of person
#define NEWS_TYPE_DELETED   3				// Data1 = IP
#define NEWS_TYPE_STOLEN	4				// Data1 = IP
#define NEWS_TYPE_COMPUTERSHUTDOWN 5		// Data1 = IP			
#define NEWS_TYPE_COMPUTERDESTROYED 6		// Data1 = IP			All databanks formatted first


// ============================================================================


class News : public UplinkObject
{

protected:

	char *details;

public:

	Date date;
	char headline [SIZE_NEWS_HEADLINE];
	
	int NEWSTYPE;
	char data1 [SIZE_NEWS_DATA];
	char data2 [SIZE_NEWS_DATA];

public:

	News ();
	~News ();

	void SetDate     ( Date *newdate );
	void SetHeadline ( char *newheadline );
	void SetDetails  ( char *newdetails );

	char *GetDetails ();

	void SetData ( int newNEWSTYPE, char *newdata1 = NULL, char *newdata2 = NULL );

	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	
	char *GetID ();
	int   GetOBJECTID ();

};



#endif


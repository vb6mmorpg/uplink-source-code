// CompanyUplink.cpp: implementation of the CompanyUplink class.
//
//////////////////////////////////////////////////////////////////////

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"

#include "world/world.h"
#include "world/company/mission.h"
#include "world/company/news.h"
#include "world/company/companyuplink.h"

#include "mmgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CompanyUplink::CompanyUplink() : Company ()
{

	SetName ( "Uplink" );

}

CompanyUplink::~CompanyUplink()
{

	DeleteLListData ( (LList <UplinkObject *> *) &missions );
	DeleteLListData ( (LList <UplinkObject *> *) &sw_sales );
	DeleteLListData ( (LList <UplinkObject *> *) &hw_sales );
	DeleteLListData ( (LList <UplinkObject *> *) &news     );

}

void CompanyUplink::CreateMission ( Mission *mission )
{

	UplinkAssert (mission);

	if ( mission->createdate.GetYear () == 1000 )
		mission->SetCreateDate ( &(game->GetWorld ()->date) );

	// Insert the mission in date order

	for ( int i = 0; i < missions.Size (); ++i ) {
		if ( mission->createdate.After ( &(missions.GetData (i)->createdate) ) ) {
			missions.PutDataAtIndex ( mission, i );
			return;
		}
	}

	// If we get here, put the mission at the end of the list
	missions.PutDataAtEnd ( mission );
	
}

void CompanyUplink::CreateMission ( char *employer, int payment, char *description, char *details, char *fulldetails )
{

	Mission *mission = new Mission ();

	mission->SetEmployer ( employer );
	mission->SetPayment ( payment );
	mission->SetDescription ( description );
	mission->SetDetails ( details );
	mission->SetFullDetails ( fulldetails );
	
	if ( mission->createdate.GetYear () == 1000 )
		mission->SetCreateDate ( &(game->GetWorld ()->date) );

	// Insert the mission in date order

	for ( int i = 0; i < missions.Size (); ++i ) {
		if ( mission->createdate.Before ( &(missions.GetData (i)->createdate) ) ) {
			missions.PutDataAtIndex ( mission, i );
			return;
		}
	}

	// If we get here, put the mission at the end of the list
	missions.PutDataAtEnd ( mission );
		
}

Mission *CompanyUplink::GetMission ( int index )
{

	if ( missions.ValidIndex (index) )    return missions.GetData (index);
	else								  return NULL;

}

Sale *CompanyUplink::GetSWSale ( int index )
{

	if ( sw_sales.ValidIndex (index) )	  return sw_sales.GetData (index);
	else								  return NULL;

}

Sale *CompanyUplink::GetHWSale ( int index )
{

	if ( hw_sales.ValidIndex (index) )	  return hw_sales.GetData (index);
	else								  return NULL;

}

News *CompanyUplink::GetNews ( int index )
{

	if ( news.ValidIndex (index) )		return news.GetData (index);
	else								return NULL;

}

void CompanyUplink::CreateHWSale ( Sale *newsale )
{
	
	UplinkAssert (newsale);
	hw_sales.PutData ( newsale );

}

void CompanyUplink::CreateSWSale ( Sale *newsale )
{
	
	UplinkAssert (newsale);
	sw_sales.PutData ( newsale );

}

void CompanyUplink::CreateNews ( News *newnews )
{

	UplinkAssert ( newnews );

	if ( newnews->date.GetYear () == 1000 )
		newnews->SetDate ( &(game->GetWorld ()->date) );

	news.PutDataAtStart ( newnews );

}

bool CompanyUplink::Load ( FILE *file )
{

	LoadID ( file );

	if ( !Company::Load ( file ) ) return false;

	if ( !LoadLList ( (LList <UplinkObject *> *) &missions,  file ) ) return false;
	if ( !LoadLList ( (LList <UplinkObject *> *) &hw_sales,  file ) ) return false;
	if ( !LoadLList ( (LList <UplinkObject *> *) &sw_sales,  file ) ) return false;
	if ( !LoadLList ( (LList <UplinkObject *> *) &news,	   file ) ) return false;

	LoadID_END ( file );

	return true;

}

void CompanyUplink::Save  ( FILE *file )
{

	SaveID ( file );

	Company::Save ( file );

	SaveLList ( (LList <UplinkObject *> *) &missions,  file );
	SaveLList ( (LList <UplinkObject *> *) &hw_sales,  file );
	SaveLList ( (LList <UplinkObject *> *) &sw_sales,  file );
	SaveLList ( (LList <UplinkObject *> *) &news,	   file );

	SaveID_END ( file );

}

void CompanyUplink::Print ()
{

	Company::Print ();

	PrintLList ( (LList <UplinkObject *> *) &missions  );
	PrintLList ( (LList <UplinkObject *> *) &hw_sales  );
	PrintLList ( (LList <UplinkObject *> *) &sw_sales  );
	PrintLList ( (LList <UplinkObject *> *) &news	   );

}

void CompanyUplink::Update ()
{
}

char *CompanyUplink::GetID ()
{
	
	return "CPNY_UPL";

}

int CompanyUplink::GetOBJECTID ()
{

	return OID_COMPANYUPLINK;

}

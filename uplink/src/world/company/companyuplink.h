
/*

  This is a specific version of the company class, 
  Modified to represent the company Uplink only.
	
  */

#ifndef _included_companyuplink_h
#define _included_companyuplink_h

// ============================================================================

#include "world/company/company.h"

class Mission;
class Sale;
class News;

// ============================================================================



class CompanyUplink : public Company  
{

public:

	LList <Mission *> missions;
	LList <Sale *>	  hw_sales;				// Hardware
	LList <Sale *>	  sw_sales;				// Software
	LList <News *>	  news;

public:

	CompanyUplink();
	virtual ~CompanyUplink();

	void CreateMission ( Mission *mission );							
	void CreateMission ( char *employer, int payment, char *description, 
						 char *details, char *fulldetails );	

	void CreateHWSale ( Sale *newsale );
	void CreateSWSale ( Sale *newsale );
	void CreateNews   ( News *newnews );

	Mission   *GetMission  ( int index );
	Sale	  *GetSWSale   ( int index );
	Sale	  *GetHWSale   ( int index );
	News	  *GetNews	   ( int index );

	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	void Update ();
	
	char *GetID ();
	int   GetOBJECTID ();

};

#endif 

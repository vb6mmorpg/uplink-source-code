
/*
	World class object
	
		Stores a database of information representing
		the game world, including companies, people, 
		computers, missions etc.

  */


#ifndef _included_world_h
#define _included_world_h

// ============================================================================

#include <stdio.h>

#include "tosser.h"

#include "app/uplinkobject.h"
#include "world/date.h"
#include "world/scheduler/eventscheduler.h"
#include "world/generator/plotgenerator.h"
#include "world/generator/demoplotgenerator.h"

class VLocation;
class Company;
class Computer;
class Person;
class Mission;
class Player;
class GatewayDef;

// ============================================================================



class World : public UplinkObject
{

protected:

	Date nextupdate;

public:

	Date date;
	EventScheduler scheduler;
    PlotGenerator plotgenerator;
    DemoPlotGenerator demoplotgenerator;

	BTree <VLocation *>  locations;
	BTree <Company *>	 companies;
	BTree <Computer *>	 computers;
	BTree <Person *>	 people;	 

	DArray <char *>		  passwords;					// No need to serialise
	DArray <GatewayDef *> gatewaydefs;					// No need to serialise

public:

	World();
	virtual ~World();

	// Database management functions

	VLocation *CreateVLocation ( char *ip, int phys_x, int phys_y );
	bool       VerifyVLocation ( char *ip, int phys_x, int phys_y );
	Company   *CreateCompany   ( char *name );
	Computer  *CreateComputer  ( char *name, char *companyname, char *ip );
	Person    *CreatePerson	   ( char *name, char *localhost );
	void	   CreatePassword  ( char *password );
		
	void CreateVLocation  ( VLocation  *vlocation );	
	void CreateCompany    ( Company    *company );
	void CreateComputer   ( Computer   *computer );
	void CreatePerson     ( Person     *person );	
	void CreateGatewayDef ( GatewayDef *newdef );
	
	VLocation  *GetVLocation  ( char *ip );				//  These all return NULL  
	Company    *GetCompany	  ( char *name );			//  if the specified object
	Computer   *GetComputer   ( char *name );			//  is not found in the database
	Person     *GetPerson     ( char *name );			//  (check before dereferencing) 
	char	   *GetPassword   ( int index );					
	GatewayDef *GetGatewayDef ( char *name );

	Player    *GetPlayer     ();						//  Asserts that player exists

	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	void Update ();
	
	char *GetID ();

};


#endif 

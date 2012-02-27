// WorldGenerator.h: interface for the WorldGenerator class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _included_worldgenerator_h
#define _included_worldgenerator_h

// ============================================================================

#include "gucci.h"
#include "tosser.h"

class VLocation;
class Company;
class Computer;
class Person;
class Agent;
class Player;
class Mission;
class Sale;
class Button;

// ============================================================================



class WorldGenerator  
{

protected:

	static Image *worldmapmask;					// Used to determine legal computer positions
	
public:

	static void Initialise ();							// Sets up the worldmapmask
    static void Shutdown();                             // Clears up memory used
    
	// Top level functions for generating groups of data

	static void GenerateAll ();						// This is the main entry function, called in a NewGame

	static void GenerateRandomWorld ();	
	static void GenerateSpecifics ();
    
    static void LoadDynamics ();                   // Loads things that change every time - LANs etc
    static void LoadDynamicsGatewayDefs ();        // Loads GatewayDefs
	
	// Middle level functions for generating specific items of data

	static Player *GeneratePlayer ( char *handle );
	static void	   GenerateLocalMachine ();

	static void GenerateCompanyUplink ();
	static void GenerateUplinkPublicAccessServer ();
	static void GenerateUplinkInternalServicesSystem ();
	static void GenerateUplinkTestMachine ();
	static void GenerateUplinkCreditsMachine ();

	static void GenerateCompanyGovernment ();
	static void GenerateGlobalCriminalDatabase ();
	static void GenerateInternationalSocialSecurityDatabase ();
	static void GenerateCentralMedicalDatabase ();
	static void GenerateGlobalIntelligenceAgency ();
	static void GenerateInternationalAcademicDatabase ();
	static void GenerateInterNIC ();
	static void GenerateStockMarket ();
	
    static void GenerateProtoVision ();
    static void GenerateOCP ();
    static void GenerateSJGames ();
    static void GenerateIntroversion ();

	static void GenerateSimpleStartingMissionA ();
	static void GenerateSimpleStartingMissionB ();

	// Low level functions for generating random items of data
	// They all add the data into the World object automatically, and then return it

	static VLocation *GenerateLocation ();			
	static Company   *GenerateCompany ();
	static Company   *GenerateCompany_Bank ();
	static Computer  *GenerateComputer ( char *companyname );
	static Person    *GeneratePerson ();
	static Agent	 *GenerateAgent ();

    static Computer  *GenerateComputer                ( char *companyName, int TYPE );
	static Computer  *GeneratePublicAccessServer	  ( char *companyname );
	static Computer  *GenerateInternalServicesMachine ( char *companyname );
	static Computer  *GenerateCentralMainframe        ( char *companyname );
	static Computer  *GeneratePublicBankServer        ( char *companyname );
	static Computer	 *GenerateEmptyFileServer		  ( char *companyname );				// Used in StealResearch missions
    static Computer  *GenerateLAN                     ( char *companyname );
	static Computer  *GeneratePersonalComputer		  ( char *personname );
	static Computer  *GenerateVoicePhoneSystem		  ( char *personname );
	

	// Low level functions for "fleshing out" specific items of data

	static Company   *GenerateCompany ( char *companyname, int size, int TYPE, int growth, int alignment );

	// Functions which return a random entry
	static VLocation *GetRandomLocation ();
	static Company   *GetRandomCompany  ();
	static Computer  *GetRandomComputer ();
	static Computer  *GetRandomComputer ( int TYPE );				// TYPE is a bitfield
	static Person	 *GetRandomPerson   ();
	static Agent	 *GetRandomAgent	();
	static Mission   *GetRandomMission  ();

	static Computer  *GetRandomLowSecurityComputer ( int TYPE );		// No proxys, no firewalls
	
	// Misc

	static void GenerateValidMapPos ( int &x, int &y );				// Generates map co-ordinates inland

	static void ReplaceAdminCompanies ( Person *person );           // For each company that the person is currently an administrator, repalce him by a new administrator
	static void ReplaceInvalidCompanyAdmins ( );                    // Replace dead or in jail company administrators
	
	static void UpdateSoftwareUpgrades ();

	// Not needed anymore since the Gateway store the GatewayDef
	//static bool VerifyPlayerGateway ();
	//static void VerifyPlayerGatewayCloseDialog ( Button *button );

};

#endif

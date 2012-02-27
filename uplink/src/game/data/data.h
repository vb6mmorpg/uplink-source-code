
/*
	Uplink Game Data File
	=====================

	  This file will contain all variables that can affect gameplay-
	  for example, company names and sizes, password lists, average cost
	  of software/hardware, difficulty of certain mission types etc.

	  It might be big ;)

	*/
 
#ifndef _included_gamedata_h
#define _included_gamedata_h

#include "app/globals.h"

/*  ===========================================================================
	Starting conditions of world

	Used in : WorldGenerator, Game + many others

	*/

#define		WORLD_START_DATE				0, 0, 0, 24, 2, 2010	
#define		GAME_START_DATE					0, 0, 0, 24, 3, 2010	// ;)


#define		COMPANYSIZE_AVERAGE				20
#define		COMPANYSIZE_RANGE				20
#define		COMPANYGROWTH_AVERAGE			10						// percent
#define		COMPANYGROWTH_RANGE				20						// percent
#define		COMPANYALIGNMENT_AVERAGE		0
#define		COMPANYALIGNMENT_RANGE			50


/*	===========================================================================
	Starting conditions of player

	Used in : WorldGenerator + many others

	*/


#define		PLAYER_START_BALANCE			3000
#define		PLAYER_START_CREDITRATING		10
#define		PLAYER_START_UPLINKRATING		0
#define		PLAYER_START_NEUROMANCERRATING  5

#define		PLAYER_START_GATEWAYNAME		"Gateway ALPHA       "
#define		PLAYER_START_CPUTYPE			"CPU ( 60 Ghz )"
#define		PLAYER_START_MODEMSPEED			1
#define		PLAYER_START_MEMORYSIZE			24


/*	===========================================================================
	Number of things to create at the start of a new game

	Used in : WorldGenerator

	*/


#define		NUM_STARTING_COMPANIES			30
#define		NUM_STARTING_BANKS				7
#define		NUM_STARTING_PEOPLE				50
#define		NUM_STARTING_AGENTS				30
#define		NUM_STARTING_MISSIONS			20

#define		NUM_STARTING_PHOTOS				20
#define     NUM_STARTING_VOICES             4


/*  ===========================================================================
	Ticks/Time required for various tasks

	(Ticks Value represents a scale factor to multiply by - 
	eg data of size 10 would take 10*TICKSREQUIRED_DELETE tick to delete)

    (Time value is in ms)

	Used in : TaskManager, ConsoleScreen

	*/


#define		TICKSREQUIRED_COPY				45
#define		TICKSREQUIRED_DELETE			9
#define		TICKSREQUIRED_DECRYPT			90
#define		TICKSREQUIRED_DEFRAG			3					// (Per memory slot)
#define		TICKSREQUIRED_DICTIONARYHACKER	0.2f				// (Num ticks required to test 1 word)
#define		TICKSREQUIRED_LOGDELETER		60					// (for entire log)
#define		TICKSREQUIRED_LOGUNDELETER		60					// (for entire log)
#define		TICKSREQUIRED_LOGMODIFIER		50					// (for entire log)
#define		TICKSREQUIRED_ANALYSEPROXY		50
#define		TICKSREQUIRED_DISABLEPROXY		100
#define		TICKSREQUIRED_ANALYSEFIREWALL	40
#define		TICKSREQUIRED_DISABLEFIREWALL	80
#define		TICKSREQUIRED_BYPASSCYPHER		0.1					// (Per element in a widthXheight element array)
#define		TICKSREQUIRED_SCANLANSYSTEM		70					// (To scan one LAN system)
#define		TICKSREQUIRED_SCANLANLINKS		100					// (For all 1024 ports to be scanned)
#define		TICKSREQUIRED_SPOOFLANSYSTEM	100					
#define     TICKSREQUIRED_FORCELANLOCK      100
#define		TICKSREQUIRED_LANSCAN			300					// (To scan all LAN systems)

#define     TIMEREQUIRED_DELETEONELOG           300             // Used in ConsoleScreen
#define     TIMEREQUIRED_DELETEONEGIGAQUAD      150             //


/*	===========================================================================
	Trace speeds of various computers	
	
	(Value represents average time to trace one link in a connection, in Seconds)
	(-1 = no trace made)
	 
	Used in : WorldGenerator

	*/

#define		TRACESPEED_VARIANCE							0.1				//(ie 10%)

#define		TRACESPEED_PUBLICACCESSSERVER				-1
#define		TRACESPEED_INTERNALSERVICESMACHINE			15
#define		TRACESPEED_CENTRALMAINFRAME					5
#define		TRACESPEED_PUBLICBANKSERVER					5
#define     TRACESPEED_LAN                              5

#define		TRACESPEED_UPLINK_INTERNALSERVICESMACHINE	5
#define		TRACESPEED_UPLINK_TESTMACHINE				30
#define		TRACESPEED_UPLINK_PUBLICACCESSSERVER		-1

#define		TRACESPEED_GLOBALCRIMINALDATABASE					10
#define		TRACESPEED_INTERNATIONALSOCIALSECURITYDATABASE		15
#define		TRACESPEED_CENTRALMEDICALDATABASE					25
#define		TRACESPEED_GLOBALINTELLIGENCEAGENCY					5
#define		TRACESPEED_INTERNATIONALACADEMICDATABASE			35
#define		TRACESPEED_INTERNIC									15
#define		TRACESPEED_STOCKMARKET								20
#define		TRACESPEED_PROTOVISION								30



#define     TRACESPEED_MODIFIER_NOACCOUNT               0.1              // Player does not have an account on bounce system
#define     TRACESPEED_MODIFIER_HASACCOUNT              0.7              // Player has an account on the bounce system
#define     TRACESPEED_MODIFIER_ADMINACCESS             1.0              // Player has admin access on bounce system
#define     TRACESPEED_MODIFIER_CENTRALMAINFRAME        1.3              // Player is routing through a central mainframe
#define     TRACESPEED_MODIFIER_PUBLICBANKSERVERADMIN   1.6              // Player has routing through a bank with admin        


/*  ===========================================================================
	Hack Difficulties of various computers

	(Value represents average number of ticks required to decrypt one letter)
	
	 Used in : WorldGenerator

  */

#define		HACKDIFFICULTY_VARIANCE							0.15		//(ie 15%)
 
#define		HACKDIFFICULTY_PUBLICACCESSSERVER				6
#define		HACKDIFFICULTY_INTERNALSERVICESMACHINE			45
#define		HACKDIFFICULTY_CENTRALMAINFRAME					80
#define		HACKDIFFICULTY_PUBLICBANKSERVER					100
#define		HACKDIFFICULTY_PUBLICBANKSERVER_ADMIN			300
#define		HACKDIFFICULTY_LOCALMACHINE						20

#define     HACKDIFFICULTY_LANTERMINAL                      75
#define     HACKDIFFICULTY_LANAUTHENTICATIONSERVER          150
#define     HACKDIFFICULTY_LANLOGSERVER                     300
#define     HACKDIFFICULTY_LANMODEM                         200
#define     HACKDIFFICULTY_LANMAINSERVER                    500

#define		HACKDIFFICULTY_UPLINK_INTERNALSERVICESMACHINE	300
#define		HACKDIFFICULTY_UPLINK_TESTMACHINE				30
#define		HACKDIFFICULTY_UPLINK_PUBLICACCESSSERVER		30

#define     HACKDIFFICULTY_ARCCENTRALMAINFRAME                      600
#define     HACKDIFFICULTY_ARUNMORCENTRALMAINFRAME                  600
#define		HACKDIFFICULTY_GLOBALCRIMINALDATABASE					180
#define		HACKDIFFICULTY_INTERNATIONALSOCIALSECURITYDATABASE		120
#define		HACKDIFFICULTY_CENTRALMEDICALDATABASE					120
#define		HACKDIFFICULTY_GLOBALINTELLIGENCEAGENCY					450
#define		HACKDIFFICULTY_INTERNATIONALACADEMICDATABASE			90
#define		HACKDIFFICULTY_INTERNIC									70
#define		HACKDIFFICULTY_STOCKMARKET								120
#define		HACKDIFFICULTY_PROTOVISION								-1                  // Unhackable



#define     MINCOMPANYSIZE_MONITOR                      1
#define     MINCOMPANYSIZE_PROXY                        8
#define     MINCOMPANYSIZE_FIREWALL                     10

#define     MAXCOMPANYSIZE_WARNINGMAIL                  5
#define     MAXCOMPANYSIZE_FINE                         12


/*
 
   Modifier used to change hackdifficulty depending on security level 
	Eg - going after level 1 is much harder than going after level 10
	(The value is a factor which is multiplied by the hackdifficulty)

										Security	Level	  -  1    2    3    4    5    6    7    8    9    10
	*/

const float	HACKDIFFICULTY_SECURITYMODIFIER []		=		{ 0.0f, 2.0f, 1.5f, 1.2f, 1.0f, 1.0f, 0.9f, 0.7f, 0.5f, 0.5f, 0.5f };


/*	===========================================================================
	Probabilities of different mission types being created
	by the mission generator.

	Used in : MissionGenerator

                                      Agent Rating:       0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16
  */


const int PROB_MISSION_STEALFILE []         =          { 30, 40, 30, 15,  5, 30, 15, 10,  0,  0,  0,  5,  5,  5,  5,  5,  5  };
const int PROB_MISSION_DESTROYFILE []       =          { 30, 40, 30, 15, 40, 10, 15,  5,  0,  0,  0,  5,  5,  5,  5,  5,  5  };
const int PROB_MISSION_CHANGEDATA []        =          { 25, 15, 25, 60, 40, 30, 20, 15, 15,  5,  0,  5,  5,  5,  5,  5,  5  };
const int PROB_MISSION_FINDDATA []          =          { 15,  5, 15, 10, 15, 30, 15, 15,  5,  0,  0,  5,  5,  5,  5,  5,  5  };
const int PROB_MISSION_REMOVECOMPUTER []    =          {  0,  0,  0,  0,  0,  0, 35, 25, 20,  5, 10, 20, 20, 20, 20, 20, 20  }; 
const int PROB_MISSION_CHANGEACCOUNT []     =          {  0,  0,  0,  0,  0,  0,  0, 30, 20, 25, 30, 20, 20, 20, 20, 20, 20  }; 
const int PROB_MISSION_REMOVEUSER []        =          {  0,  0,  0,  0,  0,  0,  0,  0, 40, 25, 30, 20, 20, 20, 20, 20, 20  }; 
const int PROB_MISSION_FRAMEUSER []         =          {  0,  0,  0,  0,  0,  0,  0,  0,  0, 40, 30, 20, 20, 20, 20, 20, 20  };

const int PROB_MISSION_REMOVECOMPANY []     =          {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0  }; 
const int PROB_MISSION_SPECIAL []           =          {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0  };
const int PROB_MISSION_TRACEUSER []         =          {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0  }; 


#define		PROB_GENERATETRACEHACKERMISSION			30			// ie X% of crimes result in a trace hacker mission


/*	===========================================================================
	Average payment for different missions

	This figure is multiplied by the difficulty

	Used in : MissionGenerator

	*/


#define		PAYMENT_MISSION_VARIANCE					0.3				// ie 30% of payment

#define		PAYMENT_MISSION_STEALFILE					900
#define		PAYMENT_MISSION_STEALALLFILES				1500
#define		PAYMENT_MISSION_DESTROYFILE					800
#define		PAYMENT_MISSION_DESTROYALLFILES				1400

#define		PAYMENT_MISSION_FINDDATA					1000
#define		PAYMENT_MISSION_FINDDATA_FINANCIAL			1200

#define		PAYMENT_MISSION_CHANGEDATA					1000
#define		PAYMENT_MISSION_CHANGEDATA_ACADEMIC			1000
#define		PAYMENT_MISSION_CHANGEDATA_SOCIAL			1200
#define		PAYMENT_MISSION_CHANGEDATA_CRIMINAL			1500

#define		PAYMENT_MISSION_FRAMEUSER					2200
#define		PAYMENT_MISSION_TRACEUSER					1800
#define		PAYMENT_MISSION_CHANGEACCOUNT				1700
#define		PAYMENT_MISSION_REMOVECOMPUTER				1600
#define		PAYMENT_MISSION_REMOVECOMPANY				2000
#define		PAYMENT_MISSION_REMOVEUSER					1900


/*
    Payment for special missions

    */


#define     PAYMENT_SPECIALMISSION_BACKFIRE             15000
#define     PAYMENT_SPECIALMISSION_TRACER               10000
#define     PAYMENT_SPECIALMISSION_TAKEMETOYOURLEADER   30000
#define     PAYMENT_SPECIALMISSION_ARCINFILTRATION      30000
#define     PAYMENT_SPECIALMISSION_COUNTERATTACK        50000

#define     PAYMENT_SPECIALMISSION_MAIDENFLIGHT         10000
#define     PAYMENT_SPECIALMISSION_DARWIN               15000
#define     PAYMENT_SPECIALMISSION_SAVEITFORTHEJURY     20000
#define     PAYMENT_SPECIALMISSION_SHINYHAMMER          30000
#define     PAYMENT_SPECIALMISSION_GRANDTOUR            50000

#define     PAYMENT_SPECIALMISSION_MOLE                 50000


/*  ===========================================================================
	Minimum rating required for mission types

	Used in : MissionGenerator

	*/

#define		DIFFICULTY_MISSION_VARIANCE						2				// Random Normal Number variance

#define		MINDIFFICULTY_MISSION_STEALFILE					2
#define		MINDIFFICULTY_MISSION_STEALALLFILES				5

#define		MINDIFFICULTY_MISSION_DESTROYFILE				2
#define		MINDIFFICULTY_MISSION_DESTROYALLFILES			5

#define		MINDIFFICULTY_MISSION_FINDDATA					2
#define		MINDIFFICULTY_MISSION_FINDDATA_FINANCIAL		5

#define		MINDIFFICULTY_MISSION_CHANGEDATA				3
#define		MINDIFFICULTY_MISSION_CHANGEDATA_ACADEMIC		3
#define		MINDIFFICULTY_MISSION_CHANGEDATA_SOCIAL			4
#define		MINDIFFICULTY_MISSION_CHANGEDATA_CRIMINAL		5

#define		MINDIFFICULTY_MISSION_FRAMEUSER					9
#define		MINDIFFICULTY_MISSION_TRACEUSER					7
#define		MINDIFFICULTY_MISSION_TRACEUSER_BANKFRAUD		8
#define		MINDIFFICULTY_MISSION_CHANGEACCOUNT				7
#define		MINDIFFICULTY_MISSION_REMOVECOMPUTER			6
#define		MINDIFFICULTY_MISSION_REMOVECOMPANY				8
#define		MINDIFFICULTY_MISSION_REMOVEUSER				8



/*  ===========================================================================
	Skills of NPC agents

	Used in : Agent AI code

	*/

#define		AGENT_UPLINKRATINGAVERAGE						7
#define		AGENT_UPLINKRATINGVARIANCE						7

#define		MINREQUIREDRATING_DELETELOGLEVEL1				2
#define		MINREQUIREDRATING_DELETELOGLEVEL2				3
#define		MINREQUIREDRATING_DELETELOGLEVEL3				4

#define		MINREQUIREDRATING_UNDELETELOGLEVEL1				7		
#define		MINREQUIREDRATING_UNDELETELOGLEVEL2				7	
#define		MINREQUIREDRATING_UNDELETELOGLEVEL3				9

#define		MINREQUIREDRATING_HACKBANKSERVER				6
#define		MINREQUIREDRATING_HACKGOVERNMENTCOMPUTER		8


/*  ===========================================================================
	Time taken for events to happen 
	(in minutes)

	Used in : ConsequenceGenerator, installing Hardware etc

	*/


#define		MINUTES								* 1
#define		HOURS								* 60 MINUTES
#define		DAYS								* 24 HOURS


#define		TIME_TOPAYLEGALFINE					7 DAYS								// You must pay before this time or face legal action

#define		TIME_LEGALACTION					3 HOURS								// They come to arrest you after this time
#define		TIME_LEGALACTION_WARNING			2 MINUTES							// You are given warning this many minutes before hand

#define		TIME_TACTICALACTION					5 MINUTES							// They send in the heavies to pick you up.  Nasty.
#define		TIME_TACTICALACTION_WARNING			1 MINUTES							// You hear them at the door

#define		TIME_TOINSTALLHARDWARE				14 HOURS							// Time for engineer to visit your gateway
#define		TIME_TOINSTALLHARDWARE_WARNING		30 MINUTES							// He mails you 30 minutes before finishing.
#define		TIME_TOCHANGEGATEWAY				24 HOURS							// No warnings.  New gateway is now ready.

#define		TIME_TOEXPIRELOGS					40 DAYS								// After this time logs will be deleted
#define		TIME_TOEXPIRENEWS					30 DAYS								// After this time news will be deleted
#define		TIME_TOEXPIREMISSIONS				30 DAYS								// After this time mission will be withdrawn

#define     TIME_TODEMOGAMEOVER                 0 MINUTES                           // Player gets this long AFTER reaching the max demo rating

#define     TIME_TOCOVERBANKROBBERY             2 MINUTES                           // Player has this long to cover his tracks

#define     TIME_REVELATIONREPRODUCE            3 MINUTES                           // Doubles in size this often
#define     TIME_ARCBUSTED_WITHPLAYER           15 MINUTES                          // Player has this long to plant Revelation
#define     TIME_ARCBUSTED_WITHOUTPLAYER        10 MINUTES                          // Player must defend against Revelation for this long

#define		FREQUENCY_GENERATENEWMISSION		12 HOURS
#define		FREQUENCY_CHECKFORSECURITYBREACHES	8 HOURS
#define		FREQUENCY_CHECKMISSIONDUEDATES		1 DAYS
#define		FREQUENCY_GIVEMISSIONTONPC			8 HOURS
#define		FREQUENCY_EXPIREOLDSTUFF			7 DAYS
#define		FREQUENCY_ADDINTERESTONLOANS		30 DAYS
#define     FREQUENCY_DEMOGENERATENEWMISSION    4 HOURS


/*  ===========================================================================
	Stats of different upgrades 

	Used in : Generators

	*/


struct ComputerUpgrade
{
	char *name;	
	int TYPE;								// Unused in HW
	int cost;
	int size;
	int data;								// SW version, HW speed or HW capacity or HW bandwidth etc
	char *description;
};


#define NUM_STARTINGSOFTWAREUPGRADES 73
#define NUM_STARTINGHARDWAREUPGRADES 19

extern const ComputerUpgrade SOFTWARE_UPGRADES  [];
extern const ComputerUpgrade HARDWARE_UPGRADES  [];

const ComputerUpgrade *GetSoftwareUpgrade ( char *name );
const ComputerUpgrade *GetHardwareUpgrade ( char *name );


/*  ===========================================================================
	Costs and price information

  */


#define		COST_UPLINK_PERMONTH			300
#define		COST_UPLINK_NEWGATEWAY			1000

#define		GATEWAY_PARTEXCHANGEVALUE		0.75				// Percentage of purchase value



#define		SMALLLOAN_MAX					3000
#define		SMALLLOAN_INTEREST				0.2

#define		MEDIUMLOAN_MAX					7000
#define		MEDIUMLOAN_INTEREST				0.4

#define		LARGELOAN_MAX					10000
#define		LARGELOAN_INTEREST				0.7

#define		MAXLOAN_INTEREST				1.0	



/*	===========================================================================
	Uplink ratings

	Used in : Rating

	*/

struct UplinkRating 
{
	char *name;
	int score;
};

#define NUM_UPLINKRATINGS		17
#define NUM_NEUROMANCERRATINGS	11

extern const UplinkRating UPLINKRATING		 [];
extern const UplinkRating NEUROMANCERRATING  [];


extern const int  NEUROMANCERCHANGE [];							// One int for each mission type - N rating changes by this much
																// (multiplied by mission difficulty)

#define NEUROMANCERCHANGE_GATEWAYNEARMISS	150					// Feds sieze your old gateway and you get away scott free
#define UPLINKCHANGE_GATEWAYNEARMISS		-30

#define NEUROMANCERCHANGE_ROBBANK           0
#define UPLINKCHANGE_ROBBANK                150


/*	===========================================================================
    Restrictions for demo game

    */


#define     DEMO_MAXUPLINKRATING      				4				
#define     DEMO_MAXGATEWAY                         2



/*  ===========================================================================
    Purity Control

    */

#define     WAREZ_MAXUPLINKRATING                   5
#define     TIME_TOWAREZGAMEOVER                    15 MINUTES                
#define     WAREZ_MAXPLAYTIME                       60 * 60 * 1000             // 60 mins in milliseconds


/*	===========================================================================
	Fixed IP addresses

	Used all over the place

	*/


#define		IP_LOCALHOST						"127.0.0.1"
#define		IP_INTERNIC							"458.615.48.651"
#define		IP_ACADEMICDATABASE					"443.65.765.2"
#define		IP_GLOBALCRIMINALDATABASE			"785.234.87.124"
#define		IP_SOCIALSECURITYDATABASE			"653.76.235.432"
#define		IP_CENTRALMEDICALDATABASE			"432.543.12.544"
#define		IP_STOCKMARKETSYSTEM				"456.789.159.459"
#define     IP_PROTOVISION                      "284.345.42.283"
#define     IP_OCP                              "265.125.767.1"
#define     IP_SJGAMES                          "849.23.459.24"
#define     IP_INTROVERSION                     "128.128.128.128"

#define		IP_UPLINKPUBLICACCESSSERVER			"234.773.0.666"
#define		IP_UPLINKCREDITSMACHINE				"128.185.0.2"
#define		IP_UPLINKINTERNALSERVICES			"128.185.0.8"
#define		IP_UPLINKTESTMACHINE				"128.185.0.4"

#define     NAME_UPLINKINTERNALSERVICES         "Uplink Internal Services System"      // Mistake here, everyone else is
                                                                                       // called 'xxxx Internal Services Machine'
                                                                                       // But fixing this would invalidate all 
                                                                                       // existing user files


/*  ===========================================================================
	Physical Locations of Gateways

	Used when the player selects his local gateway

	*/

struct PhysicalGatewayLocation
{
	char *city;
	char *country;
	int x;
	int y;
};

#define NUM_PHYSICALGATEWAYLOCATIONS 8

extern const PhysicalGatewayLocation PHYSICALGATEWAYLOCATIONS [];

#define NUM_PHYSICALGATEWAYLOCATIONS_DEFCON 8

extern const PhysicalGatewayLocation PHYSICALGATEWAYLOCATIONS_DEFCON [];



/*	===========================================================================
	Other

	Used in :				:\

	*/


#define		PERCENTAGE_PEOPLEWITHCONVICTIONS			20	
#define		PERCENTAGE_AGENTSWITHCONVICTIONS			40								

#define     REVELATION_RELEASEUNCONTROLLED				20

#define		LAN_SUBNETRANGE								1024
#define		LAN_LINKPORTRANGE							1024

#define     RADIOTRANSMITTER_MINRANGE                   140               // Ghz
#define     RADIOTRANSMITTER_MAXRANGE                   180               // Ghz

#endif

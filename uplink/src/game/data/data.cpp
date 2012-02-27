
#include "stdafx.h"

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "game/data/data.h"

#include "mmgr.h"


/*	===========================================================================
	Uplink ratings

	Used in : Rating 


								Rating name		    Score 
								-----------         -----					*/


const UplinkRating 
UPLINKRATING [] =  {			
								"Unregistered",		0,      
                                "Registered",       1,
								"Beginner",			2,     
								"Novice",			6,     
								"Confident",		15,    
                                "Intermediate",		35,    
								"Skilled",			60,    
								"Experienced",		90,    
								"Knowledgeable",	125,   
								"Uber-Skilled",		160,   
								"Professional",		220,   
								"Elite",			300,   
								"Mage",				400,   
								"Expert",			600,   
								"Veteren",			1000,  
								"Techno-mage",		1500,  
								"TERMINAL",			2500   
																	};



const UplinkRating 
NEUROMANCERRATING [] = {		"Morally bankrupt",	-1024,
								"Sociopathic",  	-512,
								"Indiscriminate",	-128,
								"Notorious",		-32,
								"Untrustworthy",	-16,
								"Neutral",  		0,
								"Aggressive",		32,
								"Single minded",	64,
								"Activist", 		256,
								"Anarchic",     	1024,
								"Revolutionary",	2048,
																	};


const int 
NEUROMANCERCHANGE [] = {		
								0,					// Special
								0,					// Steal file
								5,					// Destroy file
								0,					// Find data
							   -5,					// Change data
							  -20,					// Frame user
							  -30,					// Trace user
							  -10,					// Change account
							   30,					// Remove computer
							   60,					// Remove company
							  -60					// Remove user
	
																	};

/*  ===========================================================================
	Physical locations of gateways

									City						Country				X		Y
									----						-------				-		-
  */


const PhysicalGatewayLocation 
PHYSICALGATEWAYLOCATIONS [] = {


									"London",					"United Kingdom",	282,		69,
									"Tokyo",					"Japan",			514,		104,
									"Los Angeles",				"USA",				91,			125,
									"New York",					"USA",				171,		98,
									"Chicago",					"USA",				138,		88,
									"Moscow",					"Russia",			331,		70,
									"Hong Kong",				"China",			485,		133,
									"Sydney",					"Australia",		547,		244

																	};

const PhysicalGatewayLocation 
PHYSICALGATEWAYLOCATIONS_DEFCON [] = {


									"London",					"United Kingdom",	295,		75,
									"Tokyo",					"Japan",			527,		103,
									"Los Angeles",				"USA",				99,			106,
									"New York",					"USA",				175,		94,
									"Chicago",					"USA",				151,		91,
									"Moscow",					"Russia",			356,		65,
									"Hong Kong",				"China",			483,		126,
									"Sydney",					"Australia",		545,		226

																	};



/*	===========================================================================
	Upgrade details

	Describes the stats of all software and hardware items

		SOFTWARETYPE_NONE		0
		SOFTWARETYPE_FILEUTIL	1
		SOFTWARETYPE_HWDRIVER	2
		SOFTWARETYPE_SECURITY	3
		SOFTWARETYPE_CRACKERS	4
		SOFTWARETYPE_BYPASSERS	5
		SOFTWARETYPE_LANTOOL	6
		SOFTWARETYPE_HUDUPGRADE 9
		SOFTWARETYPE_OTHER		10

							Name						TYPE	Cost	Size	Data	Description
							----						----	----	----	----	-----------

*/


const ComputerUpgrade 
SOFTWARE_UPGRADES [] = {


							"Decrypter",				4,		800,	2,		1,  	"DECRYPTER V1.0\n" 
                                                                                        "TYPE: software.Cracker\n\n"
                                                                                        "Decrypts any file encrypted with an Encrypter V1.0",

							"Decrypter",				4,		1200,	2,		2,  	"DECRYPTER V2.0\n"
                                                                                        "TYPE: software.Cracker\n\n"
                                                                                        "Decrypts any file encrypted with an Encrypter V2.0 or V1.0",

							"Decrypter",				4,		1600,	2,		3,  	"DECRYPTER V3.0\n"
                                                                                        "TYPE: software.Cracker\n\n"
                                                                                        "Decrypts any file encrypted with an Encrypter V3.0\n"
                                                                                        "or below",

							"Decrypter",				4,		2200,	2,		4,  	"DECRYPTER V4.0\n"
                                                                                        "TYPE: software.Cracker\n\n"
                                                                                        "Decrypts any file encrypted with an Encrypter V4.0\n"
                                                                                        "or below",

							"Decrypter",				4,		3000,	3,		5,  	"DECRYPTER V5.0\n"
                                                                                        "TYPE: software.Cracker\n\n"
                                                                                        "Decrypts any file encrypted with an Encrypter V5.0\n"
                                                                                        "or below",

							"Decrypter",				4,		7000,	4,		6,  	"DECRYPTER V6.0\n"
                                                                                        "TYPE: software.Cracker\n\n"
                                                                                        "Decrypts any file encrypted with an Encrypter V6.0\n"
                                                                                        "or below",

							"Decrypter",				4,		15000,	5,		7,  	"DECRYPTER V7.0\n"  
                                                                                        "TYPE: software.Cracker\n\n"
                                                                                        "Decrypts any file encrypted with an Encrypter V7.0\n"
                                                                                        "or below",
                           


							"Decypher",					4,		3000,	2,		1,		"DECYPHER V1.0\n"
                                                                                        "TYPE: software.Cracker\n\n"
                                                                                        "Breaks through Encryption Cyphers\n"
                                                                                        "Uses a Quadratic Decryption Algorithm to force crack an Elliptic-curve Encryption Cypher",

							"Decypher",					4,		5000,	2,		2,		"DECYPHER V2.0\n"
                                                                                        "TYPE: software.Cracker\n\n"
                                                                                        "Breaks through Encryption Cyphers\n"
                                                                                        "This is a faster implementation of the Quadratic Decryption Algorithm",

							"Decypher",					4,		8000,	2,		3,		"DECYPHER V3.0\n"
                                                                                        "TYPE: software.Cracker\n\n"
                                                                                        "Breaks through Encryption Cyphers\n"
                                                                                        "This is the fastest version of this item of Software",

							"Dictionary_Hacker",		4,		1000,	4,		1,  	"DICTIONARY HACKER V1.0\n"
                                                                                        "TYPE: software.Cracker\n\n"
                                                                                        "A cheap and fast method of breaking passwords\n"
                                                                                        "This software tool tries every word in a ten-thousand word dictionary as a password. "
                                                                                        "It is faster than a Password Breaker, but not guaranteed to find an answer.",
/*
							"Encryption_Bypass",		5,		6000,	1,		1,  	"ENCRYPTION BYPASS V1.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses encryption systems with a security level of 1\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Encryption_Bypass",		5,		8000,	1,		2,  	"ENCRYPTION BYPASS V2.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses encryption systems with a security level of 2 or below\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Encryption_Bypass",		5,		10000,	1,		3,  	"ENCRYPTION BYPASS V3.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses encryption systems with a security level of 3 or below\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Encryption_Bypass",		5,		12000,	1,		4,  	"ENCRYPTION BYPASS V4.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses encryption systems with a security level of 4 or below\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Encryption_Bypass",		5,		14000,	1,		5,  	"ENCRYPTION BYPASS V5.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses encryption with a security level of 5 or below\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",
*/
							"File_Copier",				1,		100,	1,		1,  	"FILE COPIER V1.0\n"
                                                                                        "TYPE: software.FileUtil\n\n"
                                                                                        "Copies programs and data from one databank to another\n"
                                                                                        "Standard software tool given free to all Agents",

							"File_Deleter",				1,		100,	1,		1,  	"FILE DELETER V1.0\n"
                                                                                        "TYPE: software.FileUtil\n\n"
                                                                                        "Deletes programs and data from a databank\n"
                                                                                        "Standard software tool given free to all Agents",

							"Defrag",					1,		5000,	2,		1,		"DEFRAG\n"
																						"TYPE: software.FileUtil\n\n"
																						"Pushes all of your files together at the top of your memory banks\n"
																						"and frees up large blocks of space for use",

							"Firewall_Bypass",			5,		3000,	1,		1,  	"FIREWALL BYPASS V1.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses firewall systems with a security level of 1\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Firewall_Bypass",			5,		4000,	1,		2,  	"FIREWALL BYPASS V2.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses firewall systems with a security level of 2 or below\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Firewall_Bypass",			5,		6000,	1,		3,  	"FIREWALL BYPASS V3.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses firewall systems with a security level of 3 or below\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Firewall_Bypass",			5,		8000,	1,		4,  	"FIREWALL BYPASS V4.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses firewall systems with a security level of 4 or below\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Firewall_Bypass",			5,		10000,	1,		5,  	"FIREWALL BYPASS V5.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses firewall systems with a security level of 5 or below\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Firewall_Disable",			3,		2000,	1,		1,  	"FIREWALL DISABLE V1.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Disables a firewall with a security level of 1\n"
                                                                                        "This action will be detected immediately",

							"Firewall_Disable",			3,		3000,	1,		2,  	"FIREWALL DISABLE V2.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Disables a firewall with a security level of 2 or below\n"
                                                                                        "This action will be detected immediately",

							"Firewall_Disable",			3,		4000,	1,		3,  	"FIREWALL DISABLE V3.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Disables a firewall with a security level of 3 or below\n"
                                                                                        "This action will be detected immediately",

							"Firewall_Disable",			3,		6000,	2,		4,  	"FIREWALL DISABLE V4.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Disables a firewall with a security level of 4 or below\n"
                                                                                        "This action will be detected immediately",

							"Firewall_Disable",			3,		8000,	3,		5,  	"FIREWALL DISABLE V5.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Disables a firewall with a security level of 5 or below\n"
                                                                                        "This action will be detected immediately",

							"HUD_ConnectionAnalysis",	9,		20000,	3,		1,  	"CONNECTION ANALYSIS V1.0\n"
                                                                                        "TYPE: software.HUDUpgrade\n\n"
                                                                                        "Upgrades your HUD so that you can see the security systems installed on your target computer, "
                                                                                        "and launch Bypass software at them",

                            "HUD_IRC-Client",           9,      4000,   2,      1,      "IRC CLIENT V1.0\n"
                                                                                        "TYPE: software.HUDUpgrade\n\n"
                                                                                        "Upgrades your HUD with a new screen, in which you can participate in Internet Relay Chat. "
                                                                                        "This allows you to talk to other hackers for help.\n"
                                                                                        "You will also need an Internet Connection on your local machine.",

							"HUD_MapShowTrace",			9,		5000,	1,		1,  	"MAP SHOW TRACE V1.0\n"
                                                                                        "TYPE: software.HUDUpgrade\n\n"
                                                                                        "Upgrades your HUD so that your map shows the progress of any trace on your connection.\n"
                                                                                        "Replaces the Trace Tracker.",

							"HUD_LANView",			    9,		50000,	5,		1,  	"LAN VIEW V1.0\n"
                                                                                        "TYPE: software.HUDUpgrade\n\n"
                                                                                        "Upgrades your HUD so that you can analyse and connect to Local Area Network (LAN) systems.\n"
                                                                                        "This upgrade is required before you can install and run any LAN software.",

							"IP_Lookup",				10,		500,	1,		1,  	"IP LOOKUP V1.0\n"
                                                                                        "TYPE: software.Other\n\n"
                                                                                        "Converts a raw IP number into an Internet address and adds it to your links",

							"IP_Probe",					10,		2000,	3,		1,  	"IP PROBE V1.0\n"
                                                                                        "TYPE: software.Other\n\n"
                                                                                        "Scans a given IP address for security systems and reports the types in use",

							"IP_Probe",					10,		4000,	3,		2,  	"IP PROBE V2.0\n"
                                                                                        "TYPE: software.Other\n\n"
                                                                                        "Scans a given IP address for security systems and reports the types in use and their version information",

							"IP_Probe",					10,		5000,	3,		3,  	"IP PROBE V3.0\n"
                                                                                        "TYPE: software.Other\n\n"
                                                                                        "Scans a given IP address for security systems and reports the types in use, their version information and their status",

							"LAN_Probe",				6,		15000,	3,		1,		"LAN PROBE V1.0\n"
																						"TYPE: Software.LAN\n\n"
																						"Scans a single system on a LAN, and the links running from it.",

							"LAN_Probe",				6,		20000,	3,		2,		"LAN PROBE V2.0\n"
																						"TYPE: Software.LAN\n\n"
																						"Scans a single system on a LAN, and the links running from it.",

							"LAN_Probe",				6,		30000,	4,		3,		"LAN PROBE V3.0\n"
																						"TYPE: Software.LAN\n\n"
																						"Scans a single system on a LAN, and the links running from it.",

							"LAN_Scan",					6,		10000,	2,		1,		"LAN SCAN V1.0\n"
																						"TYPE: Software.LAN\n\n"
																						"Scans an entire LAN, looking for systems",

							"LAN_Scan",					6,		15000,	2,		2,		"LAN SCAN V2.0\n"
																						"TYPE: Software.LAN\n\n"
																						"Scans an entire LAN, looking for systems",

							"LAN_Scan",					6,		25000,	2,		3,		"LAN SCAN V3.0\n"
																						"TYPE: Software.LAN\n\n"
																						"Scans an entire LAN, looking for systems",

							"LAN_Spoof",				6,		20000,	2,		1,		"LAN SPOOF V1.0\n"
																						"TYPE: Software.LAN\n\n"
																						"Spoofs a system on the network, fooling the network itself\n"
																						"into believing you are that system.\n"
																						"Works on level 1 systems only.",

							"LAN_Spoof",				6,		30000,	3,		2,		"LAN SPOOF V2.0\n"
																						"TYPE: Software.LAN\n\n"
																						"Spoofs a system on the network, fooling the network itself\n"
																						"into believing you are that system.\n"
																						"Works on level 1 and 2 systems.",

							"LAN_Spoof",				6,		45000,	5,		3,		"LAN SPOOF V3.0\n"
																						"TYPE: Software.LAN\n\n"
																						"Spoofs a system on the network, fooling the network itself\n"
																						"into believing you are that system.\n"
																						"Works on all LAN systems.",

                            "LAN_Force",                6,      15000,  2,      1,      "LAN FORCE V1.0\n"
                                                                                        "TYPE: Software.LAN\n\n"
                                                                                        "This tool will force open any lock system you might\n"
                                                                                        "encounter on a LAN system.\n"
                                                                                        "The sys Admin will probably notice this.",

                            "LAN_Force",                6,      20000,  3,      2,      "LAN FORCE V2.0\n"
                                                                                        "TYPE: Software.LAN\n\n"
                                                                                        "This tool will force open any lock system you might\n"
                                                                                        "encounter on a LAN system.\n"
                                                                                        "The sys Admin will probably notice this.",

                            "LAN_Force",                6,      25000,  4,      3,      "LAN FORCE V3.0\n"
                                                                                        "TYPE: Software.LAN\n\n"
                                                                                        "This tool will force open any lock system you might\n"
                                                                                        "encounter on a LAN system.\n"
                                                                                        "The sys Admin will probably notice this.",
                                                                                        
                            "Log_Deleter",				3,		500,	1,		1,  	"LOG DELETER V1.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Deletes the contents of an access log from a computer\n"
																						"Leaves an empty log behind",

							"Log_Deleter",				3,		1000,	1,		2,  	"LOG DELETER V2.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Deletes an access-log from a computer\n"
																						"Leaves a distinctive blank space behind",

							"Log_Deleter",				3,		2000,	1,		3,  	"LOG DELETER V3.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Copies another user's legitimate access log over the top of of your own.\n"
                                                                                        "Very difficult to detect.",	

							"Log_Deleter",				3,		4000,	1,		4,  	"LOG DELETER V4.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Deletes an access-log from a computer\n"
																						"Moves other logs up to cover empty space.\n"
                                                                                        "Impossible to detect.",

							"Log_Modifier",				3,		4000,	2,		1,  	"LOG MODIFIER V1.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Allows you to make modifications to an existing log",

							"Log_Modifier",				3,		6000,	2,		2,  	"LOG MODIFIER V2.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Allows you to make modifications to an existing log, and create new logs in blank spaces",

							"Log_UnDeleter",			3,		5000,	1,		1,  	"LOG UNDELETER V1.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Recovers previously deleted logs",

							"Monitor_Bypass",			5,		10000,	1,		1,  	"MONITOR BYPASS V1.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses a monitor with a security level of 1\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Monitor_Bypass",			5,		12000,	1,		2,  	"MONITOR BYPASS V2.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses a monitor with a security level of 2 or below\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Monitor_Bypass",			5,		16000,	1,		3,  	"MONITOR BYPASS V3.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses a monitor with a security level of 3 or below\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Monitor_Bypass",			5,		20000,	1,		4,  	"MONITOR BYPASS V4.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses a monitor with a security level of 4 or below\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Monitor_Bypass",			5,		25000,	1,		5,  	"MONITOR BYPASS V5.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses a monitor with a security level of 5 or below\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Password_Breaker",			4,		1500,	2,		1,  	"PASSWORD BREAKER V1.0\n"
                                                                                        "TYPE: software.Cracker\n\n"
                                                                                        "Hacks past password protection screens\n"
                                                                                        "Not as fast as a Dictionary Hacker but is guaranteed to find the password eventually",							

							"Proxy_Bypass",				5,		6000,	1,		1,  	"PROXY BYPASS V1.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses a proxy server with a security level of 1\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Proxy_Bypass",				5,		8000,	1,		2,  	"PROXY BYPASS V2.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses a proxy server with a security level of 2 or below\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Proxy_Bypass",				5,		12000,	1,		3,  	"PROXY BYPASS V3.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses a proxy server with a security level of 3 or below\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Proxy_Bypass",				5,		16000,	1,		4,  	"PROXY BYPASS V4.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses a proxy server with a security level of 4 or below\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Proxy_Bypass",				5,		20000,	1,		5,  	"PROXY BYPASS V5.0\n"
                                                                                        "TYPE: software.Bypasser\n\n"
                                                                                        "Actively bypasses a proxy server with a security level of 5 or below\n"
                                                                                        "All bypassers require a 'HUD:ConnectionAnalysis' upgrade first",

							"Proxy_Disable",			3,		3000,	1,		1,  	"PROXY DISABLE V1.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Disables a proxy with a security level of 1\n"
                                                                                        "This action will be detected immediately",

							"Proxy_Disable",			3,		4000,	1,		2,  	"PROXY DISABLE V2.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Disables a proxy with a security level of 2 or below\n"
                                                                                        "This action will be detected immediately",

							"Proxy_Disable",			3,		6000,	1,		3,  	"PROXY DISABLE V3.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Disables a proxy with a security level of 3 or below\n"
                                                                                        "This action will be detected immediately",

							"Proxy_Disable",			3,		8000,	2,		4,  	"PROXY DISABLE V4.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Disables a proxy with a security level of 4 or below\n"
                                                                                        "This action will be detected immediately",

							"Proxy_Disable",			3,		10000,	3,		5,  	"PROXY DISABLE V5.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Disables a proxy with a security level of 5 or below\n"
                                                                                        "This action will be detected immediately",
							
							"Trace_Tracker",			3,		300,	1,		1,  	"TRACE TRACKER V1.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Informs you on the progress of any traces on your connection\n"
																						"(No trace detected, Trace in progress, Trace complete)",

							"Trace_Tracker",			3,		600,	1,		2,  	"TRACE TRACKER V2.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Informs you on the progress of any traces on your connection\n"
																						"(No trace detected, Percentage of trace complete, Trace complete)",

							"Trace_Tracker",			3,		1400,	2,		3,  	"TRACE TRACKER V3.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Informs you on the progress of any traces on your connection\n"
																						"(No trace detected, Time remaining before trace, Trace complete)",

							"Trace_Tracker",			3,		2500,	3,		4,  	"TRACE TRACKER V4.0\n"
                                                                                        "TYPE: software.Security\n\n"
                                                                                        "Informs you on the progress of any traces on your connection\n"
																						"(No trace detected, Time remaining before trace, Trace complete)\n"
                                                                                        "Including an estimate of the exact time remaining",

							"Voice_Analyser",			10,		5000,	4,		1,		"VOICE ANALYSER V1.0\n"
                                                                                        "TYPE: software.Other\n\n"
                                                                                        "Records and analyses voice patterns for reproduction.\n"
																						"Can be used to simulate voice authorisations.",

							"Voice_Analyser",			10,		10000,	5,		2,		"VOICE ANALYSER V2.0\n"
                                                                                        "TYPE: software.Other\n\n"
                                                                                        "Records and analyses voice patterns for reproduction.\n"
																						"Can store and load voice data to and from gateway memory.\n"																						"Can be used to simulate voice authorisations."
																											
																	};

/*

			#define GATEWAYUPGRADETYPE_CPU				1
			#define GATEWAYUPGRADETYPE_MODEM			2
			#define GATEWAYUPGRADETYPE_COOLING			3
			#define GATEWAYUPGRADETYPE_MEMORY			4
			#define GATEWAYUPGRADETYPE_SECURITY			5
			#define GATEWAYUPGRADETYPE_POWER			6


							Name						TYPE	Cost	Size	Data	Description
							----						----	----	----	----	-----------
*/

const ComputerUpgrade
HARDWARE_UPGRADES [] = {

							"CPU ( 20 Ghz )",	        1,		250,	0,		20, 	"CPU ( 20 Ghz )\n\n"
                                                                                        "A slow and simple processor designed for use in parallel with many others,"
                                                                                        "in large scale super-computers",
                            "CPU ( 60 Ghz )",			1,		1000,	0,		60,		"CPU ( 60 Ghz )\n\n"
                                                                                        "An entry level processor, fitted as standard to all agent computers",
							"CPU ( 80 Ghz )",			1,		1300,	0,		80,		"CPU ( 80 Ghz )\n\n"
                                                                                        "A low budget, low speed processor",
							"CPU ( 100 Ghz )",			1,		3000,	0,		100,	"CPU ( 100 Ghz )\n\n"
                                                                                        "A reasonably fast processor, and a good upgrade choice for new agents",
							"CPU ( 120 Ghz )",			1,		5000,	0,		120,	"CPU ( 120 Ghz )\n\n"
                                                                                        "A faster processor with onboard MPU instruction handling for faster instruction execution",
							"CPU ( 150 Ghz )",			1,		8000,	0,		150,	"CPU ( 150 Ghz )\n\n"
                                                                                        "A very fast processor, ideal for cpu-intensive activity",
							"CPU ( Turbo 200 Ghz )",	1,		12000,  0,		200,	"CPU ( Turbo 200 Ghz )\n\n"
                                                                                        "A high-speed accelerated processor with unique pre-emptive instruction handling, "
                                                                                        "for use in high-priority critical systems",							

							"Modem ( 1 Gq / s )",		2,		1000,	0,		1,		"Modem ( 1 Gq / s )\n\n"
                                                                                        "An entry level modem with a low transfer rate, "
                                                                                        "fitted as standard in all new Gateway systems",
							"Modem ( 2 Gq / s )",		2,		2000,   0,		2,		"Modem ( 2 Gq / s )\n\n"
                                                                                        "An low speed modem with a fairly low transfer rate",
							"Modem ( 4 Gq / s )",		2,		4000,	0,		4,		"Modem ( 4 Gq / s )\n\n"
                                                                                        "A reasonable speed modem at a good price",
							"Modem ( 6 Gq / s )",		2,		6000,	0,		6,		"Modem ( 6 Gq / s )\n\n"
                                                                                        "A fast modem, normally sufficient for all but the highest bandwidth requirements",
                            "Modem ( 8 Gq / s )",		2,		8000,	0,		8,		"Modem ( 8 Gq / s )\n\n"
                                                                                        "A very fast modem for large files and fast access times",
							"Modem ( 10 Gq / s )",		2,		10000,	0,		10,		"Modem ( 10 Gq / s )\n\n"
                                                                                        "A blindingly fast modem which will take full advantage of the network bandwidth available",

							"Memory ( 8 Gq )",			4,		3000,	0,		8,		"Memory ( 8 Gq )\n\n"
                                                                                        "8 Gigaquads of storage space",
							"Memory ( 16 Gq )",			4,		5500,	0,		16,		"Memory ( 16 Gq )\n\n"
                                                                                        "16 Gigaquads of storage space",
							"Memory ( 24 Gq )",			4,		8000,	0,		24,		"Memory ( 24 Gq )\n\n"
                                                                                        "24 Gigaquads of storage space",
							"Memory ( 32 Gq )",			4,		11000,	0,		32,		"Memory ( 32 Gq )\n\n"
                                                                                        "32 Gigaquads of storage space",

							"Gateway Self Destruct",	5,		20000,	1,		0,		"Gateway Self Destruct\n\n"
                                                                                        "Explosives are attached to your Gateway computer, "
                                                                                        "designed to destroy any evidence.\n"
                                                                                        "This is a doomsday device that has saved many agents from jail in the past.",
							"Gateway Motion Sensor",	5,		10000,	1,		0,		"Gateway Motion Sensor\n\n"
                                                                                        "An ultra-sonic based motion sensor, which can detect when people are near your gateway. "
                                                                                        "Often used together with a Self Destruct device."

																	};



const ComputerUpgrade *GetSoftwareUpgrade ( char *name )
{

	for ( int i = 0; i < NUM_STARTINGSOFTWAREUPGRADES; ++i ) {
		const ComputerUpgrade *cu = &(SOFTWARE_UPGRADES [i]);
		UplinkAssert (cu);
		if ( strcmp ( cu->name, name ) == 0 )
			return cu;
	}

	return NULL;
		
}

const ComputerUpgrade *GetHardwareUpgrade ( char *name )
{

	for ( int i = 0; i < NUM_STARTINGHARDWAREUPGRADES; ++i ) {
		const ComputerUpgrade *cu = &(HARDWARE_UPGRADES [i]);
		UplinkAssert (cu);
		if ( strcmp ( cu->name, name ) == 0 )
			return cu;
	}

	return NULL;
		
}

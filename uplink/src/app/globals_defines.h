/*

	ALL global objects / constants are defined here

  */

#ifndef _included_globals_defines_h
#define _included_globals_defines_h

//#ifdef _DEBUG
//#include "slasher.h"
//#endif


// ============================================================================
// Global defines

                                                                 // Now directly defined in the build configuration
                                                                 // SELECT ONE OF THESE THREE
//#define     FULLGAME                                           // The finished, complete game for CD distribution
//#define     DEMOGAME                                           // Limit rating, stop story progression
//#define     TESTGAME                                           // An internal build, with disabled start sequence

#if !defined(FULLGAME) && !defined(DEMOGAME) && !defined(TESTGAME)
#error "One of FULLGAME DEMOGAME or TESTGAME must be defined"
#endif

#define     VERSION_NUMBER_INT      "10.0"                       // Only the 2 first digit after the . are effective
#define     VERSION_NAME_INT        "RELEASE"

#if defined(DEMOGAME)
#define     VERSION_NUMBER          VERSION_NUMBER_INT "DEMO"
#define     VERSION_NAME            VERSION_NAME_INT "-DEMO"
#else
#define     VERSION_NUMBER          VERSION_NUMBER_INT
#define     VERSION_NAME            VERSION_NAME_INT
#endif

#define     SAVEFILE_VERSION        "SAV62"                     // Max version is SAVZZ (due to the number of characters to read)
#define     SAVEFILE_VERSION_MIN    "SAV56"                     // Minimun Savefile version to run Uplink

// SAVEFILE_VERSION 56 is 1.31 vanilla
// SAVEFILE_VERSION 57 add world map connection saving, fix to save maxpayment and paymentmethod in mission
// SAVEFILE_VERSION 58 add winning code in game.cpp
// SAVEFILE_VERSION 59 change fread of char [] for LoadDynamicString/SaveDynamicString in world\computer\computerscreen\passwordscreen.cpp,
//                     game\gameobituary.cpp, interface\remoteinterface\remoteinterface.cpp, world\computer\bankaccount.cpp,
//                     world\computer\gateway.cpp, world\generator\plotgenerator.cpp, world\scheduler\arrestevent.cpp,
//                     world\scheduler\attemptmissionevent.cpp, world\scheduler\bankrobberyevent.cpp, world\scheduler\seizegatewayevent.cpp,
//                     world\scheduler\shotbyfedsevent.cpp, world\connection.cpp
// SAVEFILE_VERSION 60 save world map server coloring in vlocation, also savegames are now saved in redshirt2
// SAVEFILE_VERSION 61 removed type from gateway
// SAVEFILE_VERSION 62 save the world map the game is using

// Build options (define in Preprocessor directives) 
//#define		USE_SDL											// Use SDL instead of glut

#define		ALPHA				0.85f							// Alpha value used for interface
#define		ALPHA_DISABLED		0.5								// Alpha value for disabled buttons

//#define	DOCLABRELEASE									    // This version designed for DOC labs
//#define	WAREZRELEASE                                        // Purity Control

//#define	CHEATMODES_ENABLED									// Eg all-links, all-software, password-bypass etc
#if defined(FULLGAME)
// Remove code card check for all builds
//#define     CODECARD_ENABLED                                    // Do the code card lookup thingy at the start
#endif
//#define   DEBUGLOG_ENABLED                                    // Redirect std* to users/debug.log

#define     CODECARD_CHRIS                                      // Which code card to use
#define     CODECARD_TOM                                        // After V1.0 we use ALL THREE - to allow a unified
#define     CODECARD_MARK                                       // Patch that works on all copies

#if !defined(DEMOGAME)
//#define     BETA_TIMEOUT
#endif

#define     DEBUGLOG_ENABLED

#if !defined(DEMOGAME)
//#define     GAME_WINNING_CODE                                 // Show the winning code on the obituary interface and send a ingame message
                                                                // when the player win the game.
#endif

//#define     MULTIMONITOR_SUPPORT                              // Is the multi-monitor support is enabled

#if defined(FULLGAME) && !defined(WAREZRELEASE)
#define VERIFY_UPLINK_LEGIT                                     // Verify if a standard patch is put on the warez or steam version.
#endif

#endif  // _included_globals_defines_h


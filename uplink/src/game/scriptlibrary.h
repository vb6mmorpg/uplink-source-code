
/*

  Script library static class

	This class is made up of a collection of static functions, identified only by an ID number.
	These functions are designed to encode specific game events that will happen only once in 
	a game of Uplink, and always under the same situation.  They are to be used to encode the 
	storyline and special events that run behind the game.
	
  */

#ifndef _included_scriptlibrary_h
#define _included_scriptlibrary_h

#include "eclipse.h"



class ScriptLibrary  
{

protected:

	// Misc drawing functions for use in these scripts

	static void DrawConnection ( Button *button, bool highlighted, bool clicked );

public:

	static void RunScript ( int scriptindex );

	static void Script1 ();							// Test script	

	static void Script10 ();						// Bank money transfers
	static void Script11 ();						// Create new bank account
	static void Script12 ();						// Create new stock market account
    static void Script13 ();                        // Close your bank account

	static void Script15 ();						// Search for Criminal Record
	static void Script16 ();						// Search for Academic Record
	static void Script17 ();						// Search for Social Security

	// ========    Start sequence =============================================

	static void Script30 ();						// Opening caption
	static void Script31 ();						// Connection anim
	static void Script32 ();						// Begin registration
	static void Script33 ();						// Registers player as Agent
	static void Script34 ();						// Connecting to Gateway anim (1)
	static void Script35 ();						// Connecting to Gateway anim (2)
	static void Script36 ();						// Connects you to your gateway

	static void Script40 ();						// Download Uplink OS
	static void Script41 ();						// Runs new OS
	static void Script42 ();						// Creates LocalInterface HUD
	static void Script43 ();						// Starts the tutorial software

    static void Script45 ();                        // Installs a new patch
    static void Script46 ();                        // Shows release notes
    static void Script47 ();                        // Shows recommendations

	// ========================================================================

	static void Script50 ();						// Bill the player for renting another gateway
	static void Script51 ();						// Player decides not to rent a new Gateway

	static void Script60 ();						// Stop Act1Scene4agents first email from being sent (subscription)
	static void Script61 ();						// Stop Act1Scene4agents second email from being sent (ARC warning)
	static void Script62 ();						// View Act1Scene4agents first email (subscription)
	static void Script63 ();						// View Act1Scene4agents second email (ARC warning)

    static void Script70 ();                        // The player just ran the Uplink_Agent_List program!
    static void Script71 ();                        // Somebody offers the player money for the Agent list
    static void Script72 ();                        // Some agents are killed due to your selling of the agent list
    
	static void Script80 ();						// Enable LAN lock systems
	static void Script81 ();						// Disable LAN lock systems
    static void Script82 ();                        // Enable LAN Isolation bridge
    static void Script83 ();                        // Disable LAN Isolation bridge
    
    static void Script85 ();                        // Do a RadioScreen connection

	// ========================================================================

    static void Script90 ();                        // Called from login_interface.cpp to create a new game from the login screen
    static void Script91 ();                        // Called from login_interface.cpp to load a game from the login screen

    static void Script92 ();                        // Script31 after the phone dialler
    static void Script93 ();                        // Script35 after the phone dialler

};

#endif

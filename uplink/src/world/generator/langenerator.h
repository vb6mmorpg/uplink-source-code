
/*

  Lan Generator

	*/

#ifndef _included_langenerator_h
#define _included_langenerator_h

class LanComputer;
class LanComputerSystem;
class LanComputerLink;
class LanCluster;
class Computer;

/*
	Alignment values:		-1 = Left aligned
							0  = Centre aligned
							+1 = Right aligned

  */


class LanGenerator
{

public:

	static void Initialise ();

    // 
    // Test generators

    static void GenerateIsolationTest           ( LanComputer *comp );
	static void GenerateRadioTest               ( LanComputer *comp );

    
    //
	// Top level functions for generating entire networks

	static Computer  *LoadLAN					( char *filename );
	static Computer  *GenerateLAN				( char *companyname, int difficulty );			// 0 = low, 5 = high
	static void       GenerateLANCluster        ( LanComputer *comp, int difficulty );

    static void       SanityCheckLAN            ( LanComputer *comp );

    //
    // Mid level functions for generating clusters of computers

    static void     GenerateLAN_Level0Cluster   ( LanComputer *comp, LanCluster *cluster, LList <int> *usedRadioFrenquencies, int x, int y, int width, int height );
    static void     GenerateLAN_Level1Cluster   ( LanComputer *comp, LanCluster *cluster, LList <int> *usedRadioFrenquencies, int x, int y, int width, int height );
    static void     GenerateLAN_Level2Cluster   ( LanComputer *comp, LanCluster *cluster, LList <int> *usedRadioFrenquencies );
    static void     GenerateLAN_Level3Cluster   ( LanComputer *comp, LanCluster *cluster, LList <int> *usedRadioFrenquencies );
    static void     GenerateLAN_Level4Cluster   ( LanComputer *comp, LanCluster *cluster, LList <int> *usedRadioFrenquencies );

    static void     GenerateLAN_BoundingBox     ( LanComputer *comp, LanCluster *cluster, int x, int y, int width, int height );

	//
	// Low level functions for generating a single atomic item
	// All return index of the item created
    // All create new systems on cluster when required
	// All create new computer screens on comp when required

	static int GenerateRouter			( LanComputer *comp, LanCluster *cluster, int x, int y, int security );
	static int GenerateHUB				( LanComputer *comp, LanCluster *cluster, int x, int y, int security );
	static int GenerateTerminal			( LanComputer *comp, LanCluster *cluster, int x, int y, int security );
	static int GenerateLock				( LanComputer *comp, LanCluster *cluster, int x, int y, int security );
	static int GenerateAuthentication	( LanComputer *comp, LanCluster *cluster, int x, int y, int security );
	static int GenerateMainServer		( LanComputer *comp, LanCluster *cluster, int x, int y, int security );
    static int GenerateLogServer        ( LanComputer *comp, LanCluster *cluster, int x, int y, int security );
    static int GenerateIsolationBridge  ( LanComputer *comp, LanCluster *cluster, int x, int y, int security );
    static int GenerateSessionKeyServer ( LanComputer *comp, LanCluster *cluster, int x, int y, int security );
    static int GenerateModem            ( LanComputer *comp, LanCluster *cluster, int x, int y, int security, char *phoneNum = NULL, size_t phoneNumsize = 0 );
    static int GenerateRadioTransmitter ( LanComputer *comp, LanCluster *cluster, int x, int y, int security );
    static int GenerateRadioReceiver    ( LanComputer *comp, LanCluster *cluster, int x, int y, int security );
    static int GenerateFaxPrinter       ( LanComputer *comp, LanCluster *cluster, int x, int y, int security );

    // Attempts to plant the data on the system.
    // Return success / failure

    static bool HideData        ( LanComputer *comp, int systemIndex, char *data );               
    static bool HideData        ( LanComputer *comp, LanComputerSystem *system, char *data );

protected:

	static int GetRadioFrequency ( LList <int> *usedRadioFrenquencies );

};



/*
    LAN cluster class

        Allows any number of systems and links to be added
        The whole lot can then be rotated around a point
        Clusters can be merged to form larger clusters
        Clusters can be merged into a LanComputer, preserving links

  */

class LanCluster
{

public:

    DArray <LanComputerSystem *> systems;
    DArray <LanComputerLink *> links;

    int input;
    int output;

    int centreX;
    int centreY;

public:

    LanCluster ();
    ~LanCluster ();

    int AddLanSystem ( LanComputerSystem *system );
    int AddLanSystem ( int TYPE, int x, int y, int security = 2, int screenIndex = -1, int dataScreenIndex = -1 );
    int AddLanLink ( int from, float fromX, float fromY, 
                     int to, float toX, float toY, int security );
    bool VerifyLanLink ( int from, float fromX, float fromY, 
                         int to, float toX, float toY, int security );

    void Rotate ( int angle );               // 1 = 90 deg, 2 = 180 deg, 3 = 270 deg
    
    void Merge ( LanCluster *target, LanComputer *comp );
    void Merge ( LanComputer *target );

    void SetInput ( int newInput ) { input = newInput; }
    void SetOutput ( int newOutput ) { output = newOutput; }

};


#endif


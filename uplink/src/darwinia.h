
#ifndef _included_darwinia_h
#define _included_darwinia_h

#define DARWINIA_VERSION				1.0

#define DARWINIA_SOUL_DYING				0
#define DARWINIA_SOUL_RETURNING			1
#define DARWINIA_SOUL_DESTROYED			2

#define DARWINIA_LIFE_DARWINIAN			0
#define DARWINIA_LIFE_REDDARWINIAN		1
#define DARWINIA_LIFE_OFFICER			2
#define DARWINIA_LIFE_BUG				3
#define DARWINIA_LIFE_SPIDER			4
#define DARWINIA_LIFE_ANT				5
#define DARWINIA_LIFE_EGGLAYER			6
#define DARWINIA_LIFE_VIRII				7
#define DARWINIA_LIFE_CENTIPEDE			8
#define DARWINIA_LIFE_SOULDESTROYER		9
#define DARWINIA_LIFE_SQUADDIE			10
#define DARWINIA_LIFE_ENGINEER			11
#define DARWINIA_LIFE_BOMBER			12
#define DARWINIA_LIFE_ARMOUR			13
#define DARWINIA_LIFE_MAX				14

#define DARWINIA_FEATURE_FENCES			1		// Increases native defences
#define DARWINIA_FEATURE_REFINERY		2		// If M+G+Y held, produce armour
#define DARWINIA_FEATURE_GENERATOR		4		// If M+G+Y held, produce armour
#define DARWINIA_FEATURE_YARD			8		// If M+G+Y held, produce armour
#define DARWINIA_FEATURE_RECEIVER		16		// Turn dying souls into returning souls and pass them to spawn points
#define DARWINIA_FEATURE_PATTERN		32		// Decides what colour new darwinians will be
#define DARWINIA_FEATURE_UPLINK			64		// External network connection from within Darwinia itself

#define DARWINIA_STATUS_NORMAL			0
#define DARWINIA_STATUS_DELETED			1
#define DARWINIA_STATUS_CORRUPTED		2		// Spawns bugs at random
#define DARWINIA_STATUS_INFECTED		4		// Spawns virii at random
#define DARWINIA_STATUS_SPIDERS			8		// Any bugs spawned become spiders instead
#define DARWINIA_STATUS_ARMYANTS		16		// Incubators have become ant hills
#define DARWINIA_STATUS_OFFLINE			32		// Server is not presently running

class DarwiniaIsland;

class Darwinia : public UplinkObject
{
protected:
	LList <DarwiniaIsland *> islands;
	LList <char *> trunkports;

public:

	Darwinia			();
	~Darwinia			();

	void Update			();
	void Save			( FILE *file );
	bool Load			( FILE *file );
	char *GetID			();
	int  GetOBJECTID	();

	void TriggerVirus	();
	void TriggerFaith	();

	DarwiniaIsland *GetIsland ( int index );
	DarwiniaIsland *GetIsland ( char *name );
	
	int OfferSoulsForRebirth	( int numsouls );
	int OfferSoulsForProcessing ( int numsouls );

	void AddIsland ( DarwiniaIsland * newisland );
	int CountIslands();

	void LinkIslands ( char *island1, char *island2 );
	int  GetLinks ( char *island );
	char *GetLink ( char *island, int index );

	void CreateDarwinia ();
	void CreateDarwiniaDemo ();
	void CreateDarwiniaDemo2 ();
	void CreateInsurrection();

	int GetPatternCorruption ();
	int GetPatternInfection ();

	char *GetLifeformName ( int index );

protected:
	void DestroyDarwinia ();

};

class DarwiniaIsland : public UplinkObject
{
protected:
	int spawners[2];
	int souls[3];

	int life[DARWINIA_LIFE_MAX];
	int lifechange[DARWINIA_LIFE_MAX];

public:

	char name[128];
	int status;
	int features;
	int size;

public:

	DarwiniaIsland		();
	~DarwiniaIsland		();

	void SetName		( char *newname );
	void SetSpawners	( int incubators, int spawnpoints );
	int  GetIncubators	();
	int  GetSpawnPoints ();
	int  GetAntHills	();

	void SetSouls		( int dying, int returning, int destroyed );
	void AdjustSouls	( int dying, int returning, int destroyed );
	int  GetSouls		( int index );

	void SetLife		( int index, int newlife );
	void AdjustLife		( int index, int newlife );
	void RunTask		( int index );
	int  GetLife		( int index );
	int  GetLifeChange	( int index );
	void ClearLifeAdjustments ();
	void SetStatus		( int newstatus );

	void SetFeatures	( int newfeatures );
	void AddFeature		( int newfeature );
	void RemoveFeature	( int newfeature );

	int GetPushFactor	();

	void Corrupt		();
	void Restore		();
	void Infect			();
	void Disinfect		();
	void Destroy		();

	void SetSize ( int newsize );

	void Update			();
	void Save			( FILE *file );
	bool Load			( FILE *file );
	char *GetID			();
	int  GetOBJECTID	();
};
#endif

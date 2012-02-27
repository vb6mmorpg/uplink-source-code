// -*- tab-width:4 c-file-style:"cc-mode" -*-

#ifndef _included_gatewaydef_h
#define _included_gatewaydef_h

/*
	
	Complete definition of a gateway

	eg Size, position of CPUs, max memory etc

  */

#include <fstream>

#include "tosser.h"

// ============================================================================

#define SIZE_GATEWAY_NAME			128
#define SIZE_GATEWAY_DESCRIPTION	256
#define SIZE_GATEWAY_FILENAME       256


// This structure is used to store physical layout information

struct GatewayDefLocation
{
	int x;
	int y;
};

#define SIZE_GATEWAY_CPU_W			32
#define SIZE_GATEWAY_CPU_H			32

#define SIZE_GATEWAY_MEM_W			10
#define SIZE_GATEWAY_MEM_H			25

#define SIZE_GATEWAY_SEC_W			32
#define SIZE_GATEWAY_SEC_H			32

#define SIZE_GATEWAY_MODEM_W		50
#define SIZE_GATEWAY_MODEM_H		20

#define SIZE_GATEWAY_POWER_W		40
#define SIZE_GATEWAY_POWER_H		30


// ============================================================================
	

class GatewayDef : public UplinkObject
{

public:

	// id information

	char name           [SIZE_GATEWAY_NAME];
	char description    [SIZE_GATEWAY_DESCRIPTION];
    char filename       [SIZE_GATEWAY_FILENAME];
    char thumbnail      [SIZE_GATEWAY_FILENAME];
	
	// Game data definitions

	int cost;
	int maxcpus;
	int maxmemory;
	int maxupgrades;
	int maxsecurity;
	int bandwidth;

	// Physical dimensions and layout

	int width;
	int height;

	DArray <GatewayDefLocation *> cpus;
	DArray <GatewayDefLocation *> memory;
	DArray <GatewayDefLocation *> security;

	int modemX;
	int modemY;
	int powerX;
	int powerY;

public:

	GatewayDef ();
	GatewayDef ( const GatewayDef& gd );
	~GatewayDef ();

	// Loaders

	void LoadGatewayDefinition ( istream &thefile );

	// Basic data set functions

	void SetName ( char *newname );
	void SetDescription ( char *newdescription );
    
    void SetFilename ( char *newfilename );
    void SetThumbnail ( char *newthumbnail );

	void SetCost ( int newcost );
	void SetMaxMemory ( int newmaxmemory );
	void SetMaxUpgrades ( int newmaxupgrades );
	void SetMaxSecurity ( int newmaxsecurity );
	void SetBandwidth ( int newbandwidth );

	// Physical location functions

	void SetSize ( int newW, int newH );
	void SetModemPosition ( int newX, int newY );
	void SetPowerPosition ( int newX, int newY );

	// Used for sorting

	static int GatewayDefComparator ( GatewayDef *const *gatewaydef1, GatewayDef *const *gatewaydef2 );

	bool VerifyCorrectness ();

	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	
	char *GetID ();

};



#endif

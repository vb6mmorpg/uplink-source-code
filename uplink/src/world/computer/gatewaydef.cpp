// -*- tab-width:4 c-file-style:"cc-mode" -*-

#include "stdafx.h"

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"
#include "app/uplinkobject.h"

#include "world/computer/gatewaydef.h"

#include "mmgr.h"



GatewayDef::GatewayDef ()
{

	UplinkStrncpy ( name, "UnNamed", sizeof ( name ) );
	UplinkStrncpy ( description, "UnDescribed ;)", sizeof ( description ) );
    UplinkStrncpy ( filename, "None", sizeof ( filename ) );
    UplinkStrncpy ( thumbnail, "None", sizeof ( thumbnail ) );
	cost = 0;
	maxcpus = 0;
	maxmemory = 0;
	maxupgrades = 0;
	maxsecurity = 0;
	bandwidth = 0;

	width = 0;
	height = 0;
	modemX = modemY = 0;
	powerX = powerY = 0;

}

GatewayDef::GatewayDef ( const GatewayDef& gd )
   : cpus ( gd.cpus ), memory ( gd.memory ), security ( gd.security )
{

	memcpy ( name, gd.name, sizeof ( name ) );
	memcpy ( description, gd.description, sizeof ( description ) );
	memcpy ( filename, gd.filename, sizeof ( filename ) );
	memcpy ( thumbnail, gd.thumbnail, sizeof ( thumbnail ) );
	
	cost = gd.cost;
	maxcpus = gd.maxcpus;
	maxmemory = gd.maxmemory;
	maxupgrades = gd.maxupgrades;
	maxsecurity = gd.maxsecurity;
	bandwidth = gd.bandwidth;

	width = gd.width;
	height = gd.height;

	for ( int c = 0; c < gd.cpus.Size (); ++c ) 
		if ( gd.cpus.ValidIndex (c) )
			if ( gd.cpus.GetData (c) )
				cpus.PutData ( new GatewayDefLocation ( *gd.cpus.GetData (c) ), c );

	for ( int m = 0; m < gd.memory.Size (); ++m ) 
		if ( gd.memory.ValidIndex (m) )
			if ( gd.memory.GetData (m) )
				memory.PutData ( new GatewayDefLocation ( *gd.memory.GetData (m) ), m );

	for ( int s = 0; s < gd.security.Size (); ++s ) 
		if ( gd.security.ValidIndex (s) )
			if ( gd.security.GetData (s) )
				security.PutData ( new GatewayDefLocation ( *gd.security.GetData (s) ), s );

	modemX = gd.modemX;
	modemY = gd.modemY;
	powerX = gd.powerX;
	powerY = gd.powerY;

}

GatewayDef::~GatewayDef ()
{

    for ( int c = 0; c < cpus.Size (); ++c ) 
        if ( cpus.ValidIndex (c) )
            if ( cpus.GetData (c) )
                delete cpus.GetData (c);


    for ( int m = 0; m < memory.Size (); ++m ) 
        if ( memory.ValidIndex (m) )
            if ( memory.GetData (m) )
                delete memory.GetData (m);


    for ( int s = 0; s < security.Size (); ++s ) 
        if ( security.ValidIndex (s) )
            if ( security.GetData (s) )
                delete security.GetData (s);
        
}

void GatewayDef::LoadGatewayDefinition ( istream &thefile )
{

	// Basics

    thefile.getline ( name, SIZE_GATEWAY_NAME );

	thefile >> cost >> maxcpus >> maxmemory >> maxupgrades 
			>> maxsecurity >> bandwidth >> width >> height >> ws;

	thefile.getline ( description, SIZE_GATEWAY_DESCRIPTION );

	// Changed so users don't get confused about the 'you cannot be caught with this gateway' part
	if ( strstr ( description, "An embedded broad-band radio transmitter provides total anonymity for the user - you cannot be caught with this gateway." ) )
		UplinkStrncpy ( description, "A good small all rounder.", sizeof ( description ) );

	// CPU positions

	cpus.SetSize ( maxcpus );

	if ( maxcpus > 0 ) {

		char unused [8];
		thefile >> unused >> ws;

		for ( int ic = 0; ic < maxcpus; ++ic ) {
		
			GatewayDefLocation *gdl = new GatewayDefLocation ();
			thefile >> gdl->x >> gdl->y >> ws;
			cpus.PutData ( gdl, ic );
			
		}

	}

	// Memory positions

	if ( maxmemory > 0 ) {

		memory.SetSize ( maxmemory );
		
		char unused [8];
		thefile >> unused >> ws;

		for ( int i = 0; i < maxmemory; ++i ) {

			GatewayDefLocation *gdl = new GatewayDefLocation ();
			thefile >> gdl->x >> gdl->y >> ws;
			memory.PutData ( gdl, i );

		}

	}

	// Security positions

	if ( maxsecurity > 0 ) {

		security.SetSize ( maxsecurity );
		
		char unused [8];
		thefile >> unused >> ws;

		for ( int i = 0; i < maxsecurity; ++i ) {

			GatewayDefLocation *gdl = new GatewayDefLocation ();
			thefile >> gdl->x >> gdl->y >> ws;
			security.PutData ( gdl, i );

		}

	}

	// Modem position

	char unused [8];
	thefile >> unused >> modemX >> modemY >> ws;

	// Power position

	thefile >> unused >> powerX >> powerY >> ws;

}

void GatewayDef::SetName ( char *newname )
{

	UplinkAssert ( strlen (newname) < SIZE_GATEWAY_NAME );
	UplinkStrncpy ( name, newname, sizeof ( name ) );
		
}

void GatewayDef::SetDescription ( char *newdescription )
{

	UplinkAssert ( strlen (newdescription) < SIZE_GATEWAY_DESCRIPTION );
	UplinkStrncpy ( description, newdescription, sizeof ( description ) );

}

void GatewayDef::SetFilename ( char *newfilename )
{

    UplinkAssert ( strlen ( newfilename) < SIZE_GATEWAY_FILENAME );
    UplinkStrncpy ( filename, newfilename, sizeof ( filename ) );

}

void GatewayDef::SetThumbnail ( char *newthumbnail )
{

    UplinkAssert ( strlen ( newthumbnail) < SIZE_GATEWAY_FILENAME );
    UplinkStrncpy ( thumbnail, newthumbnail, sizeof ( thumbnail ) );

}

void GatewayDef::SetCost ( int newcost )
{
	
	cost = newcost;

}

void GatewayDef::SetMaxMemory ( int newmaxmemory )
{

	maxmemory = newmaxmemory;
		
}

void GatewayDef::SetMaxUpgrades ( int newmaxupgrades )
{
	
	maxupgrades = newmaxupgrades;

}

void GatewayDef::SetMaxSecurity ( int newmaxsecurity )
{
	
	maxsecurity = newmaxsecurity;

}

void GatewayDef::SetBandwidth ( int newbandwidth )
{

	bandwidth = newbandwidth;

}

void GatewayDef::SetSize ( int newW, int newH )
{

	width = newW;
	height = newH;

}

void GatewayDef::SetModemPosition ( int newX, int newY )
{

	modemX = newX;
	modemY = newY;

}

void GatewayDef::SetPowerPosition ( int newX, int newY )
{

	powerX = newX;
	powerY = newY;

}

int GatewayDef::GatewayDefComparator ( GatewayDef *const *gatewaydef1, GatewayDef *const *gatewaydef2 )
{

	if ( (*gatewaydef1)->cost > (*gatewaydef2)->cost )
		return 1;
	else if ( (*gatewaydef1)->cost < (*gatewaydef2)->cost )
		return -1;
	else
		return strcmp ( (*gatewaydef1)->name, (*gatewaydef2)->name );

}

bool GatewayDef::VerifyCorrectness ()
{

	if ( maxcpus <= 0 || maxmemory <= 0 || bandwidth <= 0 ) {
		printf ( "WARNING : GatewayDef::VerifyCorrectness, Gateway '%s' loaded from '%s' as invald values, ( maxcpus <= %d || maxmemory <= %d || bandwidth <= %d )\n", name, filename, maxcpus, maxmemory, bandwidth );
		return false;
	}
	return true;

}

static void SaveDArrayGatewayDefLocation ( DArray <GatewayDefLocation *> *darray, FILE *file )
{

	UplinkAssert ( darray );

	int size = darray->Size ();
	fwrite ( &size, sizeof(size), 1, file );

	for ( int i = 0; i < size; ++i ) {
		if ( darray->ValidIndex (i) ) {

			GatewayDefLocation *data = darray->GetData (i);

			fwrite ( &i, sizeof(i), 1, file );
			fwrite ( data, sizeof(GatewayDefLocation), 1, file );

		}
		else {

			int dummy = -1;
			fwrite ( &dummy, sizeof(dummy), 1, file );

		}
	}

}

static bool LoadDArrayGatewayDefLocation ( DArray <GatewayDefLocation *> *darray, FILE *file )
{

	if ( !darray ) {
		UplinkPrintAssert ( darray );
		return false;
	}

	int size;
	if ( !FileReadData ( &size, sizeof(size), 1, file ) ) return false;

    if ( size < 0 || size > MAX_ITEMS_DATA_STRUCTURE ) {
		UplinkPrintAbortArgs ( "WARNING: LoadDArrayGatewayDefLocation, number of items appears to be wrong, size=%d", size );
		return false;
    }

	darray->SetSize ( size );

	for ( int i = 0; i < size; ++i ) {

		int index;
		if ( !FileReadData ( &index, sizeof(index), 1, file ) ) return false;

		if ( index == -1 ) {

			// Empty space

		}
		else if ( index < 0 || index >= MAX_ITEMS_DATA_STRUCTURE ) {

			UplinkPrintAbortArgs ( "WARNING: LoadDArrayGatewayDefLocation, number of items appears to be wrong, index=%d", index );
			return false;

		}
		else {

			GatewayDefLocation *data = new GatewayDefLocation;
			if ( !FileReadData ( data, sizeof(GatewayDefLocation), 1, file ) ) {
				delete data;
				return false;
			}

			darray->PutData ( data, index );			// Insert into the correct index

		}

	}

	return true;

}

bool GatewayDef::Load  ( FILE *file )
{

	LoadID ( file );

	if ( !LoadDynamicStringStatic ( name,        sizeof(name),        file ) ) return false;
	if ( !LoadDynamicStringStatic ( description, sizeof(description), file ) ) return false;
	if ( !LoadDynamicStringStatic ( filename,    sizeof(filename),    file ) ) return false;
	if ( !LoadDynamicStringStatic ( thumbnail,   sizeof(thumbnail),   file ) ) return false;

	if ( !FileReadData ( &cost,        sizeof(cost),        1, file ) ) return false;
	if ( !FileReadData ( &maxcpus,     sizeof(maxcpus),     1, file ) ) return false;
	if ( !FileReadData ( &maxmemory,   sizeof(maxmemory),   1, file ) ) return false;
	if ( !FileReadData ( &maxupgrades, sizeof(maxupgrades), 1, file ) ) return false;
	if ( !FileReadData ( &maxsecurity, sizeof(maxsecurity), 1, file ) ) return false;
	if ( !FileReadData ( &bandwidth,   sizeof(bandwidth),   1, file ) ) return false;

	if ( !FileReadData ( &width,       sizeof(width),       1, file ) ) return false;
	if ( !FileReadData ( &height,      sizeof(height),      1, file ) ) return false;

	if ( !LoadDArrayGatewayDefLocation ( &cpus,         file ) ) return false;
	if ( !LoadDArrayGatewayDefLocation ( &memory,       file ) ) return false;
	if ( !LoadDArrayGatewayDefLocation ( &security,     file ) ) return false;

	if ( !FileReadData ( &modemX,      sizeof(modemX),      1, file ) ) return false;
	if ( !FileReadData ( &modemY,      sizeof(modemY),      1, file ) ) return false;
	if ( !FileReadData ( &powerX,      sizeof(powerX),      1, file ) ) return false;
	if ( !FileReadData ( &powerY,      sizeof(powerY),      1, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void GatewayDef::Save  ( FILE *file )
{

	SaveID ( file );

	SaveDynamicString ( name,        sizeof(name),        file );
	SaveDynamicString ( description, sizeof(description), file );
	SaveDynamicString ( filename,    sizeof(filename),    file );
	SaveDynamicString ( thumbnail,   sizeof(thumbnail),   file );

	fwrite ( &cost,        sizeof(cost),        1, file );
	fwrite ( &maxcpus,     sizeof(maxcpus),     1, file );
	fwrite ( &maxmemory,   sizeof(maxmemory),   1, file );
	fwrite ( &maxupgrades, sizeof(maxupgrades), 1, file );
	fwrite ( &maxsecurity, sizeof(maxsecurity), 1, file );
	fwrite ( &bandwidth,   sizeof(bandwidth),   1, file );

	fwrite ( &width,       sizeof(width),       1, file );
	fwrite ( &height,      sizeof(height),      1, file );

	SaveDArrayGatewayDefLocation ( &cpus,          file );
	SaveDArrayGatewayDefLocation ( &memory,        file );
	SaveDArrayGatewayDefLocation ( &security,      file );

	fwrite ( &modemX,      sizeof(modemX),      1, file );
	fwrite ( &modemY,      sizeof(modemY),      1, file );
	fwrite ( &powerX,      sizeof(powerX),      1, file );
	fwrite ( &powerY,      sizeof(powerY),      1, file );

	SaveID_END ( file );

}

char *GatewayDef::GetID ()
{

	return "GATEWAYDEF";

}

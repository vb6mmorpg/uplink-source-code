
#include "app/uplinkobject.h"
#include "app/serialise.h"

#include "game/data/data.h"

#include "world/computer/lancomputer.h"
#include "world/generator/numbergenerator.h"

#include "mmgr.h"



LanComputer::LanComputer () : Computer ()
{
}

LanComputer::~LanComputer ()
{

    DeleteDArrayData ( (DArray <UplinkObject *> *) &systems );
    DeleteDArrayData ( (DArray <UplinkObject *> *) &links );
    
}

int LanComputer::AddLanSystem ( LanComputerSystem *system )
{

    return systems.PutData ( system );

}

int LanComputer::AddLanSystem ( int TYPE, int x, int y, int security, int screenIndex, int dataScreenIndex )
{

    LanComputerSystem *system = new LanComputerSystem ();
    system->TYPE = TYPE;
    system->x = x;
    system->y = y;
	system->subnet = NumberGenerator::RandomNumber ( LAN_SUBNETRANGE );
	system->security = security;
	system->screenIndex = screenIndex;
    system->dataScreenIndex = dataScreenIndex;
    if ( TYPE == LANSYSTEM_ROUTER ) system->visible = LANSYSTEMVISIBLE_TYPE;
    return AddLanSystem ( system );

}

int LanComputer::AddLanLink ( int from, float fromX, float fromY,  
                               int to, float toX, float toY, int security )
{

    LanComputerLink *link = new LanComputerLink ();
    link->from = from;
    link->to = to;
    link->fromX = fromX;
    link->fromY = fromY;
    link->toX = toX;
    link->toY = toY;
	link->port = NumberGenerator::RandomNumber ( LAN_LINKPORTRANGE );
	link->security = security;
    return links.PutData ( link );

}

bool LanComputer::Load  ( FILE *file )
{

    LoadID( file );

    if ( !Computer::Load ( file ) ) return false;

    if ( !LoadDArray ( (DArray <UplinkObject *> *) &systems, file ) ) return false;
    if ( !LoadDArray ( (DArray <UplinkObject *> *) &links, file ) ) return false;

    LoadID_END ( file );

	return true;

}

void LanComputer::Save  ( FILE *file )
{

    SaveID ( file );

    Computer::Save ( file );

    SaveDArray ( (DArray <UplinkObject *> *) &systems, file );
    SaveDArray ( (DArray <UplinkObject *> *) &links, file );

    SaveID_END ( file );

}

void LanComputer::Print ()
{

    printf ( "LAN Computer\n" );

    PrintDArray ( (DArray <UplinkObject *> *) &systems );
    PrintDArray ( (DArray <UplinkObject *> *) &links );

    Computer::Print ();

}

void LanComputer::Update ()
{

    Computer::Update ();

}

char *LanComputer::GetID ()
{
    return "LANCOMP";
}

int LanComputer::GetOBJECTID ()
{
    return OID_LANCOMPUTER;
}

// ============================================================================


LanComputerSystem::LanComputerSystem ()
{
    TYPE = LANSYSTEM_NONE;
    x = y = 0;
    visible = LANSYSTEMVISIBLE_NONE;
	subnet = 0;
	security = 0;
	screenIndex = -1;
    dataScreenIndex = -1;
	data1 = data2 = data3 = -1;
}

LanComputerSystem::~LanComputerSystem ()
{
}

void LanComputerSystem::IncreaseVisibility( int newValue )
{

    if ( newValue > visible )
        visible = newValue;

}

bool LanComputerSystem::Load ( FILE *file )
{

    LoadID ( file );

    if ( !FileReadData ( &TYPE, sizeof(TYPE), 1, file ) ) return false;
    if ( !FileReadData ( &x, sizeof(x), 1, file ) ) return false;
    if ( !FileReadData ( &y, sizeof(y), 1, file ) ) return false;
    if ( !FileReadData ( &visible, sizeof(visible), 1, file ) ) return false;
	if ( !FileReadData ( &subnet, sizeof(subnet), 1, file ) ) return false;
	if ( !FileReadData ( &security, sizeof(security), 1, file ) ) return false;
	if ( !FileReadData ( &screenIndex, sizeof(screenIndex), 1, file ) ) return false;
    if ( !FileReadData ( &dataScreenIndex, sizeof(dataScreenIndex), 1, file ) ) return false;
	
	if ( !FileReadData ( &data1, sizeof(data1), 1, file ) ) return false;
	if ( !FileReadData ( &data2, sizeof(data2), 1, file ) ) return false;
	if ( !FileReadData ( &data3, sizeof(data3), 1, file ) ) return false;

	if ( !LoadDArray ( &validSubnets, file ) ) return false;

    LoadID_END ( file );

	return true;

}

void LanComputerSystem::Save ( FILE *file )
{

    SaveID ( file );

    fwrite ( &TYPE, sizeof(TYPE), 1, file );
    fwrite ( &x, sizeof(x), 1, file );
    fwrite ( &y, sizeof(y), 1, file );
    fwrite ( &visible, sizeof(visible), 1, file );
	fwrite ( &subnet, sizeof(subnet), 1, file );
	fwrite ( &security, sizeof(security), 1, file );
	fwrite ( &screenIndex, sizeof(screenIndex), 1, file );
    fwrite ( &dataScreenIndex, sizeof(dataScreenIndex), 1, file );

	fwrite ( &data1, sizeof(data1), 1, file );
	fwrite ( &data2, sizeof(data2), 1, file );
	fwrite ( &data3, sizeof(data3), 1, file );

	SaveDArray ( &validSubnets, file );

    SaveID_END ( file );

}

void LanComputerSystem::Print ()
{

    printf ( "LanComputerSystem\n" );
    printf ( "TYPE = %d\n", TYPE );
    printf ( "Position = %d, %d\n", x, y );
    printf ( "Visible = %d\n", visible );
	printf ( "Subnet = %d\n", subnet );
	printf ( "Security = %d\n", security );
	printf ( "ScreenIndex = %d, DataScreenIndex = %d\n", screenIndex, dataScreenIndex );
	printf ( "Data1 = %d, Data2 = %d, Data3 = %d\n", data1, data2, data3 );
	
	PrintDArray ( &validSubnets );

}

char *LanComputerSystem::GetID ()
{
    return "LANSYST";
}

int LanComputerSystem::GetOBJECTID ()
{
    return OID_LANCOMPUTERSYSTEM;
}


// ============================================================================


LanComputerLink::LanComputerLink ()
{

    visible = LANLINKVISIBLE_NONE;
    from = -1;
    to = -1;
	port = 1000;
	security = 1;
    fromX = fromY = toX = toY = 0.0f;

}

LanComputerLink::~LanComputerLink ()
{
}

void LanComputerLink::IncreaseVisibility( int newValue )
{

    if ( newValue > visible )
        visible = newValue;

}

bool LanComputerLink::Load  ( FILE *file )
{
    LoadID ( file );

    if ( !FileReadData ( &visible, sizeof(visible), 1, file ) ) return false;

    if ( !FileReadData ( &from, sizeof(from), 1, file ) ) return false;
    if ( !FileReadData ( &to, sizeof(to), 1, file ) ) return false;
	if ( !FileReadData ( &port, sizeof(port), 1, file ) ) return false;
	if ( !FileReadData ( &security, sizeof(security), 1, file ) ) return false;

    if ( !FileReadData ( &fromX, sizeof(fromX), 1, file ) ) return false;
    if ( !FileReadData ( &fromY, sizeof(fromY), 1, file ) ) return false;
    if ( !FileReadData ( &toX, sizeof(toX), 1, file ) ) return false;
    if ( !FileReadData ( &toY, sizeof(toY), 1, file ) ) return false;

    LoadID_END ( file );

	return true;

}

void LanComputerLink::Save  ( FILE *file )
{
    SaveID ( file );

    fwrite ( &visible, sizeof(visible), 1, file );

    fwrite ( &from, sizeof(from), 1, file );
    fwrite ( &to, sizeof(to), 1, file );
	fwrite ( &port, sizeof(port), 1, file );
	fwrite ( &security, sizeof(security), 1, file );

    fwrite ( &fromX, sizeof(fromX), 1, file );
    fwrite ( &fromY, sizeof(fromY), 1, file );
    fwrite ( &toX, sizeof(toX), 1, file );
    fwrite ( &toY, sizeof(toY), 1, file );

    SaveID_END ( file );

}

void LanComputerLink::Print ()
{

    printf ( "LanComputerLink\n" );

    printf ( "visible = %d\n", visible );
    printf ( "From %d to %d\n", from, to );
    printf ( "From XY = %f, %f\n", fromX, fromY );
    printf ( "to XY = %f, %f\n", toX, toY );
	printf ( "Port %d\n", port );
	printf ( "Security level %d\n", security );

}
	
char *LanComputerLink::GetID ()
{
    return "LANLINK";
}

int LanComputerLink::GetOBJECTID ()
{
    return OID_LANCOMPUTERLINK;
}


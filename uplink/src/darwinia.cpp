
#include "gucci.h"

#include "game/game.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/uplinkobject.h"
#include "app/serialise.h"


#include "world/world.h"
#include "world/computer/computer.h"
#include "world/computer/databank.h"
#include "world/person.h"

#include "world/company/news.h"
#include "world/company/companyuplink.h"

#include "world/generator/plotgenerator.h"
#include "world/generator/numbergenerator.h"
#include "world/generator/namegenerator.h"

#include "darwinia.h"

static char linklist [512];                 // This is used to return string values

//#define DARWINIA_LIFE_DARWINIAN		0
//#define DARWINIA_LIFE_REDDARWINIAN	1
//#define DARWINIA_LIFE_OFFICER			2
//#define DARWINIA_LIFE_BUG				3
//#define DARWINIA_LIFE_SPIDER			4
//#define DARWINIA_LIFE_ANT				5
//#define DARWINIA_LIFE_EGGLAYER		6
//#define DARWINIA_LIFE_VIRII			7
//#define DARWINIA_LIFE_CENTIPEDE		8
//#define DARWINIA_LIFE_SOULDESTROYER	9
//#define DARWINIA_LIFE_SQUADDIE		10
//#define DARWINIA_LIFE_ENGINEER		11
//#define DARWINIA_LIFE_BOMBER			12
//#define DARWINIA_LIFE_ARMOUR			13
//#define DARWINIA_LIFE_MAX				14

static char *lifeforms [] = {	"Darwinian",
								"Red Darwinian",
								"Officer",
								"Bug",
								"Spider",
								"Ant",
								"Spore Generator",
								"Virii",
								"Centipede",
								"Soul Destroyer",
								"Squadie",
								"Engineer",
								"Bomber",
								"Armour"};
Darwinia::Darwinia()
{
}

Darwinia::~Darwinia()
{
	DeleteLListData ( (LList <UplinkObject *> *) &islands );
}

void Darwinia::Update()
{
	for ( int i = 0; i < islands.Size(); i++ )
	{
		islands.GetData(i)->ClearLifeAdjustments();
	}
	for ( int i = 0; i < islands.Size(); i++ )
	{
		islands.GetData(i)->Update();
	}
}

void Darwinia::Save(FILE *file)
{
	SaveID ( file );
	SaveLList ( (LList <UplinkObject *> *) &islands,  file );
	SaveLList ( &trunkports,  file );
	SaveID_END ( file );
}

bool Darwinia::Load(FILE *file)
{
	LoadID ( file );
	if ( !LoadLList ( (LList <UplinkObject *> *) &islands,  file ) ) return false;
	if ( !LoadLList ( &trunkports,  file ) ) return false;
	LoadID_END ( file );

	return true;
}

void Darwinia::TriggerVirus()
{
	for ( int i = 0; i < islands.Size(); i++ )
	{
		islands.GetData(i)->Corrupt();
		islands.GetData(i)->Infect();
	}
}
void Darwinia::TriggerFaith()
{
	for ( int i = 0; i < islands.Size(); i++ )
	{
		islands.GetData(i)->Disinfect();
	}
}

void Darwinia::DestroyDarwinia()
{
}

char *Darwinia::GetID ()
{

	return "DARWNIA";

}

int Darwinia::GetOBJECTID ()
{

	return OID_DARWINIA;

}

DarwiniaIsland *Darwinia::GetIsland ( char *name )
{
	for ( int i = 0; i < islands.Size(); i++ )
	{
		if ( strcmp(islands.GetData(i)->name, name) == 0 )
			return islands.GetData(i);
	}
	return NULL;
}

DarwiniaIsland *Darwinia::GetIsland ( int index )
{
	if ( islands.ValidIndex(index) )
		return islands.GetData(index);

	return NULL;
}

int Darwinia::CountIslands ()
{
	return islands.Size();
}

int Darwinia::OfferSoulsForRebirth ( int numsouls )
{
	int bufferpattern = -1;
	
	int chanceRed = GetPatternInfection();
	int chanceBug = GetPatternCorruption();

	if ( chanceRed == -1 || chanceBug == -1 )
		return numsouls;		// There is no pattern to use :(

	int result = NumberGenerator::RandomNumber(100);
	if ( result < chanceRed )
		bufferpattern = DARWINIA_LIFE_REDDARWINIAN;
	else if ( result < (chanceRed + chanceBug) )
		bufferpattern = DARWINIA_LIFE_BUG;
	else
		bufferpattern = DARWINIA_LIFE_DARWINIAN;

	// Hand out as many souls as we can, then return how many we couldnt give away

	for ( int i = 0; i < islands.Size(); i++ )
	{
		DarwiniaIsland *island = islands.GetData(i);
		int dg = island->GetSpawnPoints();
		if ( dg > numsouls )
			dg = numsouls;

		numsouls -= dg;

		island->AdjustLife(bufferpattern,dg);

	}
	return numsouls;
}

int Darwinia::OfferSoulsForProcessing ( int numsouls )
{
	for ( int i = 0; i < islands.Size(); i++ )
	{
		DarwiniaIsland *island = islands.GetData(i);
		if ( island->features & DARWINIA_FEATURE_RECEIVER )
		{
			int taken = NumberGenerator::RandomNumber(numsouls);
			island->AdjustSouls(0,taken,0);
			numsouls -= taken;
		}
	}
	return numsouls;
}

void Darwinia::AddIsland(DarwiniaIsland *newisland)
{
	islands.PutData(newisland);
}

void Darwinia::LinkIslands(char *island1, char *island2)
{
	if ( strcmp(island1,island2) == 0 )
		return;

	char linkage[256];
	UplinkSnprintf(linkage, sizeof(linkage), "%s %s", island1, island2);

	size_t newsize = 128;
	char *newstr = new char [newsize];
	UplinkAssert ( strlen (linkage) < 128 );
	UplinkStrncpy ( newstr, linkage, newsize );

	trunkports.PutData(newstr);
}

int Darwinia::GetLinks (char *island)
{
	int count = 0;

	for ( int i = 0; i < trunkports.Size(); i++ )
	{
		char *tempname = trunkports.GetData(i);
		char from[128]; char to[128];
		sscanf(tempname, "%s %s", from, to);
		if ( strcmp(from, island) == 0 )
			count++;
		else if ( strcmp(to, island) == 0 )
			count++;

	}
	return count;
}
char *Darwinia::GetLink (char *island, int index)
{
	int count = 0;

	for ( int i = 0; i < trunkports.Size(); i++ )
	{
		char *tempname = trunkports.GetData(i);
		char from[128]; char to[128];
		sscanf(tempname, "%s %s", from, to);
		if ( strcmp(from, island) == 0 )
		{
			UplinkSnprintf(linklist, sizeof(linklist), "%s", to);
			count++;
		}
		else if ( strcmp(to, island) == 0 )
		{
			UplinkSnprintf(linklist, sizeof(linklist), "%s", from);
			count++;
		}
		if ( count == index+1 )
		{
			return linklist;
		}

	}
	UplinkSnprintf(linklist, sizeof(linklist), "unknown_location");
	return linklist;
}

int Darwinia::GetPatternCorruption()
{
	int corrupt = 0;
	int total = 0;
	for ( int i = 0; i < islands.Size(); i++ )
	{
		if ( islands.GetData(i)->features & DARWINIA_FEATURE_PATTERN )
		{
			total++;
			if ( islands.GetData(i)->status & DARWINIA_STATUS_CORRUPTED )
				corrupt++;
		}
	}

	if ( !total )
		return -1;
	else
		return corrupt * 100 / total;
}
int Darwinia::GetPatternInfection()
{
	int corrupt = 0;
	int total = 0;
	for ( int i = 0; i < islands.Size(); i++ )
	{
		if ( islands.GetData(i)->features & DARWINIA_FEATURE_PATTERN )
		{
			total++;
			if ( islands.GetData(i)->status & DARWINIA_STATUS_INFECTED )
				corrupt++;
		}
	}

	if ( !total )
		return -1;
	else
		return corrupt * 100 / total;
}

char *Darwinia::GetLifeformName ( int index )
{
	UplinkAssert ( index >= 0 && index < DARWINIA_LIFE_MAX );
	UplinkSnprintf(linklist, sizeof(linklist), "%s", lifeforms[index]);
	return linklist;
}
// ================

DarwiniaIsland::DarwiniaIsland () : UplinkObject ()
{
	spawners[0] = spawners[1] = 0;
	souls[0] = souls[1] = souls[2] = 0;
	for ( int i = 0; i < DARWINIA_LIFE_MAX; i++ )
		life[i] = 0;
	status = 0;
	features = 0;
	SetName("Unknown");
}

DarwiniaIsland::~DarwiniaIsland()
{
}

void DarwiniaIsland::SetName ( char *newname )
{
	UplinkAssert(newname);
	UplinkAssert(sizeof(newname) < 128);

	UplinkSnprintf(name, sizeof(name), "%s", newname);
}
void DarwiniaIsland::SetSpawners(int incubators, int spawnpoints)
{
	if ( status & DARWINIA_STATUS_DELETED ) { return; }
	spawners[0] = incubators;
	spawners[1] = spawnpoints;
}

int DarwiniaIsland::GetIncubators ()
{
	if ( status & DARWINIA_STATUS_DELETED ) { return 0; }
	if ( status & DARWINIA_STATUS_ARMYANTS ) { return 0; }
	return spawners[0];
}

int DarwiniaIsland::GetSpawnPoints ()
{
	if ( status & DARWINIA_STATUS_DELETED ) { return 0; }
	return spawners[1];
}

int DarwiniaIsland::GetAntHills ()
{
	if ( status & DARWINIA_STATUS_DELETED ) { return 0; }
	if ( status & DARWINIA_STATUS_ARMYANTS ) { return spawners[0]; }
	return 0;
}

void DarwiniaIsland::SetSouls(int dying, int returning, int destroyed)
{
	if ( status & DARWINIA_STATUS_DELETED ) { return; }
	souls[0] = dying;
	souls[1] = returning;
	souls[2] = destroyed;
	for ( int i = 0; i < 3; i++ )
		if ( souls[i] < 0 ) souls[i] = 0;
}
void DarwiniaIsland::AdjustSouls(int dying, int returning, int destroyed)
{
	if ( status & DARWINIA_STATUS_DELETED ) { return; }
	souls[0] = souls[0] + dying;
	souls[1] = souls[1] + returning;
	souls[2] = souls[2] + destroyed;
	for ( int i = 0; i < 3; i++ )
		if ( souls[i] < 0 ) souls[i] = 0;

}
int DarwiniaIsland::GetSouls ( int index )
{
	if ( status & DARWINIA_STATUS_DELETED ) { return souls[2]; }
	UplinkAssert(index >= 0 && index < 3);
	return souls[index];
}

void DarwiniaIsland::SetLife(int index, int newlife)
{
	if ( status & DARWINIA_STATUS_DELETED ) { return; }
	UplinkAssert(index >= 0 && index < DARWINIA_LIFE_MAX);
	if ( index == DARWINIA_LIFE_BUG && status & DARWINIA_STATUS_SPIDERS )
		index = DARWINIA_LIFE_SPIDER;

	lifechange[index] += (newlife - life[index]);
	life[index] = newlife;
}

void DarwiniaIsland::AdjustLife(int index, int newlife)
{
	if ( status & DARWINIA_STATUS_DELETED ) { return; }
	UplinkAssert(index >= 0 && index < DARWINIA_LIFE_MAX);
	if ( index == DARWINIA_LIFE_BUG && status & DARWINIA_STATUS_SPIDERS )
		index = DARWINIA_LIFE_SPIDER;
	
	lifechange[index] += newlife;
	life[index] = life[index] + newlife;
}

void DarwiniaIsland::RunTask(int index)
{
	if ( status & DARWINIA_STATUS_DELETED ) { return; }
	UplinkAssert(index >= 0 && index < DARWINIA_LIFE_MAX);

	int tasks = 0;
	tasks += (GetLife(DARWINIA_LIFE_SQUADDIE) / 5);
	tasks += GetLife(DARWINIA_LIFE_ENGINEER);

	if ( tasks >= 4 ) { return; }

	if ( index == DARWINIA_LIFE_BUG && status & DARWINIA_STATUS_SPIDERS )
		index = DARWINIA_LIFE_SPIDER;
	
	if ( index == DARWINIA_LIFE_SQUADDIE )
	{
		lifechange[index] += 5;
		life[index] = life[index] + 5;
	}
	else if ( index == DARWINIA_LIFE_ENGINEER )
	{
		lifechange[index] += 1;
		life[index] = life[index] + 1;
	}
}

int DarwiniaIsland::GetLife (int index)
{
	if ( status & DARWINIA_STATUS_DELETED ) { return 0; }
	UplinkAssert(index >= 0 && index < DARWINIA_LIFE_MAX);
	return life[index];
}
int DarwiniaIsland::GetLifeChange (int index)
{
	if ( status & DARWINIA_STATUS_DELETED ) { return 0; }
	UplinkAssert(index >= 0 && index < DARWINIA_LIFE_MAX);
	return lifechange[index];
}

void DarwiniaIsland::SetStatus(int newstatus)
{
	status = newstatus;
}

void DarwiniaIsland::SetFeatures(int newfeatures)
{
	if ( status & DARWINIA_STATUS_DELETED ) { return; }
	features = newfeatures;
}

void DarwiniaIsland::AddFeature(int newfeature)
{
	if ( status & DARWINIA_STATUS_DELETED ) { return; }
	features = features | newfeature;
}

void DarwiniaIsland::RemoveFeature(int newfeature)
{
	if ( status & DARWINIA_STATUS_DELETED ) { return; }
	if ( features & newfeature )
		features = features - newfeature;
}

void DarwiniaIsland::Corrupt()
{
	if ( status & DARWINIA_STATUS_DELETED ) { return; }
	status = status | DARWINIA_STATUS_CORRUPTED;
	int bugs = GetLife(DARWINIA_LIFE_DARWINIAN)/8;
	AdjustLife(DARWINIA_LIFE_DARWINIAN,-bugs);
	AdjustLife(DARWINIA_LIFE_BUG,bugs);
}

void DarwiniaIsland::Restore()
{
	if ( status & DARWINIA_STATUS_DELETED ) { return; }
	if ( status & DARWINIA_STATUS_CORRUPTED )
		status = status - DARWINIA_STATUS_CORRUPTED;
}

void DarwiniaIsland::Infect()
{
	if ( status & DARWINIA_STATUS_DELETED ) { return; }
	status = status | DARWINIA_STATUS_INFECTED;
	int bugs = GetLife(DARWINIA_LIFE_DARWINIAN)/8;
	AdjustLife(DARWINIA_LIFE_DARWINIAN,-bugs);
	AdjustLife(DARWINIA_LIFE_VIRII,bugs);
}

void DarwiniaIsland::Disinfect()
{
	if ( status & DARWINIA_STATUS_DELETED ) { return; }
	if ( status & DARWINIA_STATUS_INFECTED )
		status = status - DARWINIA_STATUS_INFECTED;
	if ( status & DARWINIA_STATUS_SPIDERS )
		status = status - DARWINIA_STATUS_SPIDERS;
	if ( status & DARWINIA_STATUS_ARMYANTS )
		status = status - DARWINIA_STATUS_ARMYANTS;
}

void DarwiniaIsland::Destroy()
{
	int soulcount = 0;
	int i;
	for ( i = 0; i < 3; i++ )
		soulcount += souls[i];

	for ( i = 0; i < DARWINIA_LIFE_MAX; i++ )
	{
		soulcount += GetLife(i);
		SetLife(i,0);
	}

	SetSouls(0,0,soulcount);
	status = DARWINIA_STATUS_DELETED;
	features = 0;
	SetSpawners(0,0);

}
void DarwiniaIsland::Update()
{
	Computer *comp = game->GetWorld ()->GetComputer ( NameGenerator::GenerateProjectServerName("Darwin Research Associates") );
	UplinkAssert ( comp );

	if ( comp->isrunning ) {
		if ( status & DARWINIA_STATUS_OFFLINE )
			status = status - DARWINIA_STATUS_OFFLINE; }
	else {
		status  = status & DARWINIA_STATUS_OFFLINE;
		return; }


	char dataname[SIZE_DATA_TITLE];
	UplinkSnprintf(dataname, sizeof(dataname), "Darwinia%s", name);

	if ( comp->databank.ContainsData(dataname) == false ) {
		Destroy();
		return;
	} else {
		if ( status & DARWINIA_STATUS_DELETED )
			status = status - DARWINIA_STATUS_DELETED;
	}

	// Spawn some new guys :)

	if ( status & DARWINIA_STATUS_ARMYANTS )
	{
		// Ants have hijacked our incubators :o
	}
	else
	{
		int numSpawned = life[DARWINIA_LIFE_ENGINEER] * 10 * spawners[0] / (1+NumberGenerator::RandomNumber(10));
		if ( numSpawned > souls[0] ) numSpawned = souls[0];

		// We can usurp the master pattern here, because engineers can reprogram incubators :)
		AdjustLife(DARWINIA_LIFE_DARWINIAN,numSpawned);
		souls[0] -= numSpawned;
	}

	if ( status & DARWINIA_STATUS_CORRUPTED )
	{
		int numBugs;
		// Poor little DGs turn into bugs :(
		if ( GetLife(DARWINIA_LIFE_DARWINIAN) > 0 ) {
			numBugs = NumberGenerator::RandomNumber(life[DARWINIA_LIFE_DARWINIAN]/8);
			AdjustLife(DARWINIA_LIFE_DARWINIAN,-numBugs);
			AdjustLife(DARWINIA_LIFE_BUG,numBugs);
		}
		// Red DGs are more prone to bugs due to their altered codebase
		if ( GetLife(DARWINIA_LIFE_REDDARWINIAN) > 0 ) {
			numBugs = NumberGenerator::RandomNumber(life[DARWINIA_LIFE_REDDARWINIAN]/4);
			AdjustLife(DARWINIA_LIFE_REDDARWINIAN,-numBugs);
			AdjustLife(DARWINIA_LIFE_BUG,numBugs);
		}
	}

	// Natural Aging only occurs to Darwinians
	int numDead = 0;
	if ( life[DARWINIA_LIFE_DARWINIAN] > 100 )
	{
		numDead = NumberGenerator::RandomNumber(life[DARWINIA_LIFE_DARWINIAN] / 50);
		souls[0] += numDead;
		AdjustLife(DARWINIA_LIFE_DARWINIAN, -numDead);
	}
	if ( life[DARWINIA_LIFE_REDDARWINIAN] > 100 )
	{
		numDead = NumberGenerator::RandomNumber(life[DARWINIA_LIFE_REDDARWINIAN] / 50);
		souls[0] += numDead;
		AdjustLife(DARWINIA_LIFE_REDDARWINIAN, -numDead);
	}

	if ( features & DARWINIA_FEATURE_RECEIVER )
		souls[1] = game->GetWorld ()->GetDarwinia ()->OfferSoulsForRebirth(souls[1]); 	// Process our soul rain
	else
		souls[0] = game->GetWorld ()->GetDarwinia ()->OfferSoulsForProcessing(souls[0]); // Send some souls up to the sky

	// Migration
	int migrants = 0;
	int port = NumberGenerator::RandomNumber(game->GetWorld ()->GetDarwinia ()->GetLinks(name));
	char *destname = game->GetWorld ()->GetDarwinia ()->GetLink(name, port);
	DarwiniaIsland *dest = game->GetWorld ()->GetDarwinia ()->GetIsland ( destname );

	// Only migrate if it exists, its online and its more desirable
	if ( dest && dest->GetPushFactor() < GetPushFactor() && !(dest->status & DARWINIA_STATUS_DELETED) )
	{
		if ( life[DARWINIA_LIFE_DARWINIAN] > 0 )
		{
			migrants = NumberGenerator::RandomNumber(GetPushFactor());
			if ( migrants > life[DARWINIA_LIFE_DARWINIAN] ) migrants = life[DARWINIA_LIFE_DARWINIAN];
			AdjustLife(DARWINIA_LIFE_DARWINIAN, -migrants);
			dest->AdjustLife(DARWINIA_LIFE_DARWINIAN, migrants);
		}
		if ( life[DARWINIA_LIFE_REDDARWINIAN] > 0 )
		{
			migrants = NumberGenerator::RandomNumber(GetPushFactor());
			if ( migrants > life[DARWINIA_LIFE_REDDARWINIAN] ) migrants = life[DARWINIA_LIFE_REDDARWINIAN];
			AdjustLife(DARWINIA_LIFE_REDDARWINIAN, -migrants);
			dest->AdjustLife(DARWINIA_LIFE_REDDARWINIAN, migrants);
		}
	}

}

bool DarwiniaIsland::Load(FILE *file)
{
	LoadID(file);

	if ( !LoadDynamicStringStatic ( name, sizeof(name), file ) ) return false;
	if ( !FileReadData ( &spawners[0], sizeof(spawners[0]), 1, file ) ) return false;
	if ( !FileReadData ( &spawners[1], sizeof(spawners[1]), 1, file ) ) return false;
	if ( !FileReadData ( &souls[0], sizeof(souls[0]), 1, file ) ) return false;
	if ( !FileReadData ( &souls[1], sizeof(souls[1]), 1, file ) ) return false;
	if ( !FileReadData ( &souls[2], sizeof(souls[2]), 1, file ) ) return false;
	if ( !FileReadData ( &status, sizeof(status), 1, file ) ) return false;
	if ( !FileReadData ( &features, sizeof(features), 1, file ) ) return false;
	if ( !FileReadData ( &size, sizeof(size), 1, file ) ) return false;

	for ( int i = 0; i < DARWINIA_LIFE_MAX; i++ )
	{
		if ( !FileReadData ( &life[i], sizeof(life[i]), 1, file ) ) return false;
	}

	LoadID_END(file);
	return true;
}

void DarwiniaIsland::Save(FILE *file)
{
	SaveID(file);

	SaveDynamicString ( name, sizeof(name), file );

	fwrite ( &spawners[0], sizeof(spawners[0]), 1, file );
	fwrite ( &spawners[1], sizeof(spawners[1]), 1, file );
	fwrite ( &souls[0], sizeof(souls[0]), 1, file );
	fwrite ( &souls[1], sizeof(souls[1]), 1, file );
	fwrite ( &souls[2], sizeof(souls[2]), 1, file );
	fwrite ( &status, sizeof(status), 1, file );
	fwrite ( &features, sizeof(features), 1, file );
	fwrite ( &size, sizeof(size), 1, file );

	for ( int i = 0; i < DARWINIA_LIFE_MAX; i++ )
		fwrite ( &life[i], sizeof(life[i]), 1, file );

	SaveID_END(file);
}

char *DarwiniaIsland::GetID ()
{

	return "DISLAND";

}

int DarwiniaIsland::GetOBJECTID ()
{

	return OID_DARWINIAISLAND;

}

void DarwiniaIsland::SetSize ( int newsize )
{
	size = newsize;
}

int DarwiniaIsland::GetPushFactor ( )
{
	// How much pressure is there to leave the zone
	// Higher = more emmigration

	int push;;

	push = 0;
	// High population = more emmigration
	for ( int i = 0; i < DARWINIA_LIFE_MAX; i++ )
	{
		push += life[i];
	}
	// More space = less emmigration
	//push = (5*push) / size;
	push = push/size;

	// There is a real reason to be here, so less immigration
	if ( features )
		push = push/2;

	// Something is wrong with the zone, run away!
	if ( status )
		push = push * 4;

	return push;

}

void DarwiniaIsland::ClearLifeAdjustments ()
{
	for ( int i = 0; i < DARWINIA_LIFE_MAX; i++ )
	{
		lifechange[i] = 0;
	}
}
//==========

void Darwinia::CreateDarwinia()
{
	DarwiniaIsland *garden = new DarwiniaIsland();
	garden->SetName("Garden");
	garden->SetSpawners(0,0);
	garden->SetLife(DARWINIA_LIFE_DARWINIAN,229);
	garden->SetSize(4);
	AddIsland(garden);

	DarwiniaIsland *containment = new DarwiniaIsland();
	containment->SetName("Containment");
	containment->SetSpawners(3,0);
	containment->SetLife(DARWINIA_LIFE_DARWINIAN,589);
	containment->AddFeature(DARWINIA_FEATURE_FENCES);
	containment->SetSize(20);
	AddIsland(containment);

	DarwiniaIsland *generator = new DarwiniaIsland();
	generator->SetName("Generator");
	generator->SetSpawners(2,0);
	generator->SetLife(DARWINIA_LIFE_DARWINIAN,346);
	generator->AddFeature(DARWINIA_FEATURE_GENERATOR);
	generator->SetSize(29);
	AddIsland(generator);

	DarwiniaIsland *escort = new DarwiniaIsland();
	escort->SetName("Escort");
	escort->SetSpawners(2,0);
	escort->SetLife(DARWINIA_LIFE_DARWINIAN,636);
	escort->SetSize(16);
	AddIsland(escort);

	DarwiniaIsland *mine = new DarwiniaIsland();
	mine->SetName("Mine");
	mine->SetSpawners(3,0);
	mine->SetLife(DARWINIA_LIFE_DARWINIAN,434);
	mine->AddFeature(DARWINIA_FEATURE_REFINERY);
	mine->SetSize(14);
	AddIsland(mine);

	DarwiniaIsland *yard = new DarwiniaIsland();
	yard->SetName("Yard");
	yard->SetSpawners(2,0);
	yard->SetLife(DARWINIA_LIFE_DARWINIAN,781);
	yard->AddFeature(DARWINIA_FEATURE_YARD);
	yard->SetSize(25);
	AddIsland(yard);

	DarwiniaIsland *receiver = new DarwiniaIsland();
	receiver->SetName("Receiver");
	receiver->SetSpawners(2,0);
	receiver->SetLife(DARWINIA_LIFE_DARWINIAN,646);
	receiver->AddFeature(DARWINIA_FEATURE_RECEIVER);
	receiver->SetSize(23);
	AddIsland(receiver);

	DarwiniaIsland *buffer = new DarwiniaIsland();
	buffer->SetName("Pattern_Buffer");
	buffer->SetSpawners(5,0);
	buffer->SetLife(DARWINIA_LIFE_DARWINIAN,597);
	buffer->AddFeature(DARWINIA_FEATURE_PATTERN);
	buffer->SetSize(14);
	AddIsland(buffer);

	DarwiniaIsland *biosphere = new DarwiniaIsland();
	biosphere->SetName("Biosphere");
	biosphere->SetSpawners(0,23);
	biosphere->SetLife(DARWINIA_LIFE_DARWINIAN,677);
	biosphere->SetSize(21);
	AddIsland(biosphere);

	DarwiniaIsland *temple = new DarwiniaIsland();
	temple->SetName("Temple");
	temple->SetSpawners(3,8);
	temple->SetLife(DARWINIA_LIFE_DARWINIAN,434);
	temple->AddFeature(DARWINIA_FEATURE_UPLINK);
	temple->SetSize(19);
	AddIsland(temple);

	LinkIslands("Garden",			"Containment");
	LinkIslands("Containment",		"Generator");
	LinkIslands("Containment",		"Mine");
	LinkIslands("Containment",		"Yard");
	LinkIslands("Generator",		"Escort");
	LinkIslands("Generator",		"Mine");
	LinkIslands("Generator",		"Yard");
	LinkIslands("Mine",				"Yard");
	LinkIslands("Yard",				"Receiver");
	LinkIslands("Yard",				"Pattern_Buffer");
	LinkIslands("Yard",				"Biosphere");
	LinkIslands("Receiver",			"Pattern_Buffer");
	LinkIslands("Receiver",			"Biosphere");
	LinkIslands("Pattern_Buffer",	"Biosphere");
	LinkIslands("Biosphere",		"Temple");
}

void Darwinia::CreateDarwiniaDemo()
{
	DarwiniaIsland *mine = new DarwiniaIsland();
	mine->SetName("Mine");
	mine->SetSpawners(3,0);
	mine->SetLife(DARWINIA_LIFE_DARWINIAN,434);
	mine->AddFeature(DARWINIA_FEATURE_REFINERY);
	mine->SetSize(14);
	AddIsland(mine);

	LinkIslands("Mine", "Containment");
	LinkIslands("Mine", "Generator");
	LinkIslands("Mine", "Yard");

}

void Darwinia::CreateDarwiniaDemo2()
{
	DarwiniaIsland *launchpad = new DarwiniaIsland();
	launchpad->SetName("Launchpad");
	launchpad->SetSpawners(2,4);
	launchpad->SetLife(DARWINIA_LIFE_DARWINIAN,406);
	launchpad->SetSize(14);
	AddIsland(launchpad);

	LinkIslands("Launchpad", "Mine");
	LinkIslands("Launchpad", "Biosphere");

}

void Darwinia::CreateInsurrection()
{
	DarwiniaIsland *rebellion = new DarwiniaIsland();
	rebellion->SetName("Rebellion");
	rebellion->SetSpawners(1,0);
	rebellion->SetLife(DARWINIA_LIFE_DARWINIAN,225);
	rebellion->AddFeature(DARWINIA_FEATURE_UPLINK);
	AddIsland(rebellion);

	DarwiniaIsland *database = new DarwiniaIsland();
	database->SetName("Database");
	database->SetSpawners(1,0);
	database->SetLife(DARWINIA_LIFE_DARWINIAN,680);
	AddIsland(database);

	DarwiniaIsland *recruitment = new DarwiniaIsland();
	recruitment->SetName("Recruitment");
	recruitment->SetSpawners(0,25);
	recruitment->SetLife(DARWINIA_LIFE_DARWINIAN,269);
	AddIsland(recruitment);

	DarwiniaIsland *discovery = new DarwiniaIsland();
	discovery->SetName("Discovery");
	discovery->SetSpawners(0,0);
	discovery->SetLife(DARWINIA_LIFE_DARWINIAN,839);
	AddIsland(discovery);

	DarwiniaIsland *station = new DarwiniaIsland();
	station->SetName("Station");
	station->SetSpawners(1,0);
	station->SetLife(DARWINIA_LIFE_DARWINIAN,548);
	station->AddFeature(DARWINIA_FEATURE_YARD);
	station->AddFeature(DARWINIA_FEATURE_GENERATOR);
	AddIsland(station);

	DarwiniaIsland *repository = new DarwiniaIsland();
	repository->SetName("Repository");
	repository->SetSpawners(0,8);
	repository->SetLife(DARWINIA_LIFE_DARWINIAN,225);
	repository->AddFeature(DARWINIA_FEATURE_REFINERY);
	AddIsland(repository);

	DarwiniaIsland *processor = new DarwiniaIsland();
	processor->SetName("Processor");
	processor->SetSpawners(2,0);
	processor->SetLife(DARWINIA_LIFE_DARWINIAN,762);
	processor->AddFeature(DARWINIA_FEATURE_PATTERN);
	processor->AddFeature(DARWINIA_FEATURE_FENCES);
	AddIsland(processor);

	DarwiniaIsland *annihilation = new DarwiniaIsland();
	annihilation->SetName("Annihilation");
	annihilation->SetSpawners(0,10);
	annihilation->SetLife(DARWINIA_LIFE_DARWINIAN,380);
	AddIsland(annihilation);

	DarwiniaIsland *primary = new DarwiniaIsland();
	primary->SetName("Primary_Core");
	primary->SetSpawners(0,8);
	primary->SetLife(DARWINIA_LIFE_DARWINIAN,236);
	AddIsland(primary);

	DarwiniaIsland *aftermath = new DarwiniaIsland();
	aftermath->SetName("Aftermath");
	aftermath->SetSpawners(4,0);
	aftermath->SetLife(DARWINIA_LIFE_DARWINIAN,1679);
	AddIsland(aftermath);

	DarwiniaIsland *sanctuary = new DarwiniaIsland();
	sanctuary->SetName("Sanctuary");
	sanctuary->SetSpawners(4,0);
	sanctuary->SetLife(DARWINIA_LIFE_DARWINIAN,1216);
	sanctuary->AddFeature(DARWINIA_FEATURE_FENCES);
	AddIsland(sanctuary);

	DarwiniaIsland *secondary = new DarwiniaIsland();
	secondary->SetName("Secondary_Core");
	secondary->SetSpawners(0,10);
	secondary->SetLife(DARWINIA_LIFE_DARWINIAN,812);
	AddIsland(secondary);

	DarwiniaIsland *backdoor = new DarwiniaIsland();
	backdoor->SetName("Backdoor");
	backdoor->SetSpawners(0,0);
	backdoor->SetLife(DARWINIA_LIFE_DARWINIAN,145);
	backdoor->AddFeature(DARWINIA_FEATURE_FENCES);
	AddIsland(backdoor);

	DarwiniaIsland *communications = new DarwiniaIsland();
	communications->SetName("Communications");
	communications->SetSpawners(3,0);
	communications->SetLife(DARWINIA_LIFE_DARWINIAN,872);
	AddIsland(communications);

	DarwiniaIsland *mine047 = new DarwiniaIsland();
	mine047->SetName("Mine047");
	AddIsland(mine047);

	DarwiniaIsland *mine607 = new DarwiniaIsland();
	mine607->SetName("Mine607");
	AddIsland(mine607);

	LinkIslands("Rebellion",			"Database");
	LinkIslands("Database",				"Recruitment");
	LinkIslands("Database",				"Discovery");
	LinkIslands("Database",				"Recovery");
	LinkIslands("Recruitment",			"Processor");
	LinkIslands("Recruitment",			"Repository");
	LinkIslands("Station",				"Repository");
	LinkIslands("Repository",			"Mine047");
	LinkIslands("Repository",			"Mine607");
	LinkIslands("Processor",			"Image");
	LinkIslands("Processor",			"Annihilation");
	LinkIslands("Image",				"Sanctuary");
	LinkIslands("Annihilation",			"Primary_Core");
	LinkIslands("Annihilation",			"Backdoor");
	LinkIslands("Primary_Core",			"Secondary_Core");
	LinkIslands("Primary_Core",			"Repository");
	LinkIslands("Primary_Core",			"Backdoor");
	LinkIslands("Primary_Core",			"Communications");
	LinkIslands("Aftermath",			"Secondary_Core");
	LinkIslands("Sanctuary",			"Backdoor");
	LinkIslands("Secondary_Core",		"Backdoor");
	LinkIslands("Secondary_Core",		"Communications");
	LinkIslands("Mine047",				"Biosphere");

}
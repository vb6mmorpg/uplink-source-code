
#include "stdafx.h"

#include "gucci.h"

#include "app/app.h"
#include "app/serialise.h"
#include "app/globals.h"

#include "game/game.h"
#include "game/gameobituary.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/gatewaydef.h"
#include "world/generator/numbergenerator.h"

#include "mmgr.h"

GameObituary::GameObituary ()
{

	gameoverreason = NULL;
	UplinkStrncpy ( name, " ", sizeof ( name ) );
	money = 0;
	uplinkrating = 0;
	neuromancerrating = 0;
	specialmissionscompleted = 0;

	score_peoplefucked = 0;
	score_systemsfucked = 0;
	score_highsecurityhacks = 0;

	score = 0;
	
	demogameover = false;
    warezgameover = false;
	
}

GameObituary::~GameObituary ()
{

	if ( gameoverreason ) delete [] gameoverreason;

}


void GameObituary::SetGameOverReason ( char *newreason )
{

	if ( gameoverreason ) delete [] gameoverreason;
	gameoverreason = new char [strlen(newreason)+1];
	UplinkSafeStrcpy ( gameoverreason, newreason );

}

char *GameObituary::GameOverReason ()
{

	return gameoverreason;

}

void GameObituary::SetDemoGameOver ( bool newvalue )
{

    demogameover = newvalue;

}

void GameObituary::SetWarezGameOver ( bool newvalue )
{

    warezgameover = newvalue;

}

void GameObituary::Evaluate ()
{

	UplinkStrncpy ( name, game->GetWorld ()->GetPlayer ()->handle, sizeof ( name ) );
	money = game->GetWorld ()->GetPlayer ()->GetBalance ();
	uplinkrating = game->GetWorld ()->GetPlayer ()->rating.uplinkrating;	
	neuromancerrating = game->GetWorld ()->GetPlayer ()->rating.neuromancerrating;
	specialmissionscompleted = game->GetWorld ()->plotgenerator.GetSpecialMissionsCompleted ();

	score_peoplefucked = game->GetWorld ()->GetPlayer ()->score_peoplefucked;
	score_systemsfucked = game->GetWorld ()->GetPlayer ()->score_systemsfucked;
	score_highsecurityhacks = game->GetWorld ()->GetPlayer ()->score_highsecurityhacks;

    GatewayDef *gatewaydef = game->GetWorld ()->GetPlayer ()->gateway.curgatewaydef;
    UplinkAssert (gatewaydef);
    int gatewayvalue = gatewaydef->cost;

	if ( money > 10000000 ) money = 10000000;

	int numspecialmissions = 0;
	for ( int i = 0; i < 16; ++i )
		if ( game->GetWorld ()->plotgenerator.PlayerCompletedSpecialMission (i) )
			++numspecialmissions;

	score = game->GetWorld ()->GetPlayer ()->rating.uplinkscore +
			money / 100 +
			gatewayvalue / 1000 +		
			score_peoplefucked +
			score_systemsfucked +
			score_highsecurityhacks +
			numspecialmissions * 200;

    score *= 100;
			
    if ( warezgameover ) score = 0;

}

bool GameObituary::Load   ( FILE *file )
{

	LoadID ( file );

	if ( strcmp( game->GetLoadedSavefileVer(), "SAV59" ) >= 0 ) {
		if ( !LoadDynamicStringStatic ( name, sizeof(name), file ) ) return false;
	}
	else {
		if ( !FileReadData ( name, sizeof(name), 1, file ) ) {
			name [ sizeof(name) - 1 ] = '\0';
			return false;
		}
		name [ sizeof(name) - 1 ] = '\0';
	}
	if ( !FileReadData ( &money, sizeof(money), 1, file ) ) return false;
	if ( !FileReadData ( &uplinkrating, sizeof(uplinkrating), 1, file ) ) return false;
	if ( !FileReadData ( &neuromancerrating, sizeof(neuromancerrating), 1, file ) ) return false;
	if ( !FileReadData ( &specialmissionscompleted, sizeof(specialmissionscompleted), 1, file ) ) return false;

	if ( !FileReadData ( &score_peoplefucked, sizeof(score_peoplefucked), 1, file ) ) return false;
	if ( !FileReadData ( &score_systemsfucked, sizeof(score_systemsfucked), 1, file ) ) return false;
	if ( !FileReadData ( &score_highsecurityhacks, sizeof(score_highsecurityhacks), 1, file ) ) return false;	
	if ( !FileReadData ( &score, sizeof(score), 1, file ) ) return false;

    if ( !FileReadData ( &demogameover, sizeof(demogameover), 1, file ) ) return false;
    if ( !FileReadData ( &warezgameover, sizeof(warezgameover), 1, file ) ) return false;

	if ( !LoadDynamicStringPtr ( &gameoverreason, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void GameObituary::Save   ( FILE *file )
{

	SaveID ( file );

	SaveDynamicString ( name, sizeof(name), file );
	fwrite ( &money, sizeof(money), 1, file );
	fwrite ( &uplinkrating, sizeof(uplinkrating), 1, file );	
	fwrite ( &neuromancerrating, sizeof(neuromancerrating), 1, file );
	fwrite ( &specialmissionscompleted, sizeof(specialmissionscompleted), 1, file );

	fwrite ( &score_peoplefucked, sizeof(score_peoplefucked), 1, file );
	fwrite ( &score_systemsfucked, sizeof(score_systemsfucked), 1, file );
	fwrite ( &score_highsecurityhacks, sizeof(score_highsecurityhacks), 1, file );
	fwrite ( &score, sizeof(score), 1, file );

    fwrite ( &demogameover, sizeof(demogameover), 1, file );
    fwrite ( &warezgameover, sizeof(warezgameover), 1, file );

	SaveDynamicString ( gameoverreason, file );

	SaveID_END ( file );

}

void GameObituary::Print  ()
{
	
	printf ( "Game obituary: \n" );
	printf ( "Name : %s\n", name );
	printf ( "Money %d, Uplink %d, Neuromancer %d\n", money, uplinkrating, neuromancerrating );
	printf ( "SpecialMissionsCompleted : %d\n", specialmissionscompleted );

	printf ( "Score : People fucked : %d\n", score_peoplefucked );
	printf ( "Score : Systems fucked : %d\n", score_systemsfucked );
	printf ( "Score : High Security Hacks : %d\n", score_highsecurityhacks );
	printf ( "Score %d\n", score );

    printf ( "DemoGameOver = %d\n", demogameover );
    
#ifdef WAREZRELEASE    
    printf ( "WarezGameOver = %d\n", warezgameover );
#endif

	printf ( "Game over reason = %s\n", gameoverreason );

}

void GameObituary::Update ()
{
}

char *GameObituary::GetID ()
{

	return "GOBIT";

}

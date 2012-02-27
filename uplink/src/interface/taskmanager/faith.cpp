

#include "eclipse.h"
#include "vanbakel.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/opengl_interface.h"
#include "app/globals.h"

#include "game/game.h"

#include "world/world.h"
#include "world/player.h"
#include "world/generator/numbergenerator.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/taskmanager/faith.h"

#include "mmgr.h"


void Faith::Initialise ()
{
}

void Faith::Tick ( int n )
{

    // Is revelation running?

    UplinkTask *revelation = (UplinkTask *) SvbGetTask ( "Revelation" );

    SgPlaySound ( RsArchiveFileOpen ( "sounds/faith.wav" ), "sounds/faith.wav", false );
 
    if ( revelation ) {

        if ( revelation->version <= version ) {

            revelation->RemoveInterface ();
            SvbRemoveTask ( revelation );
            SvbRemoveTask ( this );
            create_msgbox ( "Faith", "Revelation has been purged" );

        }

    }
    else {

        create_msgbox ( "Faith", "Revelation is not\nrunning locally" );
        SvbRemoveTask ( this );

    }

}


void Faith::CreateInterface ()
{

}

void Faith::RemoveInterface ()
{

}

void Faith::ShowInterface ()
{

}


bool Faith::IsInterfaceVisible ()
{
    return false;
}


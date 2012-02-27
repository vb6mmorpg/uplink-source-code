
#if 0

// MemoryInterface.cpp: implementation of the MemoryInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "eclipse.h"

#include "localinterface.h"

#include "memory_interface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void MemoryInterface::TitleClick ( Button *button )
{

	game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_MEMORY );

}

void MemoryInterface::Create ()
{

	if ( !IsVisible () ) {

		EclRegisterButton ( 449, 130, 188, 15, "MEMORY BANKS", "Remove the memory screen", "memory_title" );
		EclRegisterButtonCallback ( "memory_title", TitleClick );

	}

}

void MemoryInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "memory_title" );

	}

}

void MemoryInterface::Update ()
{
}

bool MemoryInterface::IsVisible ()
{

	return ( EclGetButton ( "memory_title" ) != NULL );

}

int MemoryInterface::ScreenID ()
{
	
	return SCREEN_MEMORY;

}

#endif

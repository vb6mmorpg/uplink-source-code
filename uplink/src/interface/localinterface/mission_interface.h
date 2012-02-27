// MissionInterface.h: interface for the MissionInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _included_missioninterface_h
#define _included_missioninterface_h

// ============================================================================

#include "interface/localinterface/localinterfacescreen.h"

class Mission;

// ============================================================================


class MissionInterface : public LocalInterfaceScreen  
{

protected:

	int index;
	time_t timesync;
	Mission *mission;

protected:

	static void TitleClick ( Button *button );
	static void AbandonClick ( Button *button );
	static void ReplyClick ( Button *button );

public:

	MissionInterface ();
	~MissionInterface ();

	void SetMission ( int index );

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

};

#endif 

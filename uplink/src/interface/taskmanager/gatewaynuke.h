
/*
	
  Gatwaye Nuke software driver

	Checks you have gate-nuke hardware,
	then takes down your gateway, destroying
	all evidence

  */

#ifndef _included_gatewaynuke_h
#define _included_gatewaynuke_h

// ============================================================================

#include "interface/taskmanager/uplinktask.h"

#define GATEWAYNUKE_STATUS_NONE			0
#define GATEWAYNUKE_STATUS_START		1
#define GATEWAYNUKE_STATUS_AREYOUSURE	2
#define GATEWAYNUKE_STATUS_FAILED		3

// ============================================================================


class GatewayNuke : public UplinkTask
{

protected:

	int status;

protected:

	static void TitleDraw ( Button *button, bool highlighted, bool clicked );

	static void YesClick ( Button *button );
	static void NoClick ( Button *button );

	static void NukeGateway ();												// Does the dirty work

public:

	GatewayNuke ();
	~GatewayNuke ();

	void SetTarget ( UplinkObject *uo, char *uos, int uoi );
	void MoveTo ( int x, int y, int time_ms );				

	void Initialise ();     
	void Tick ( int n );           
	
	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	bool IsInterfaceVisible ();     

};



#endif


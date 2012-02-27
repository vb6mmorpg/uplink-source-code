
/*

  Security Bypass

	Similar to disable but this software runs continuously, 
	bypassing a single proxy server.

	This is a generic task designed to run on any security system

  */



#ifndef _included_proxybypass_h
#define _included_proxybypass_h

#include "world/vlocation.h"

#include "interface/taskmanager/uplinktask.h"


class SecurityBypass : public UplinkTask
{

protected:

	static void PauseClick ( Button *button );
	static void CloseClick ( Button *button );
	static void BypassClick ( Button *button );

	static void StatusLightDraw ( Button *button, bool highlighted, bool clicked );
	static void ImageButtonDraw ( Button *button, bool highlighted, bool clicked );

	static bool ShouldDraw ( int pid );

protected:

	char ip [SIZE_VLOCATION_IP];				// Target IP

public:

	int TYPE;												// Security system type
	int status;												// 0 = static, 1 = working

public:

	SecurityBypass ();
	SecurityBypass ( int newTYPE );
	~SecurityBypass ();

	void SetTYPE ( int newTYPE );

	void PauseCurrentBypass ();
	void ResumeCurrentBypass ();

	void EndCurrentBypass ();

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



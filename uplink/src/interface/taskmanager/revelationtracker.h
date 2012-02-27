
/*

  Revelation Tracker

	Sits on the players computer
    Informs him when computers are infected with Revelation

  */


#ifndef _included_revelationtracker_h
#define _included_revelationtracker_h



#include "interface/taskmanager/uplinktask.h"


class RevelationTracker : public UplinkTask
{

protected:

    time_t timesync;
    int height;                                             // In rows

protected:
    
    static void TitleClick ( Button *button );
    static void CloseClick ( Button *button );

    static void MainBoxDraw ( Button *button, bool highlighted, bool clicked );

public:

    RevelationTracker ();
    ~RevelationTracker ();

    void MoveTo ( int x, int y, int time_ms );

	void Initialise ();     
	void Tick ( int n );	

	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	bool IsInterfaceVisible ();    

};



#endif


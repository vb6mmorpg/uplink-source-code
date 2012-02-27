
/*

  Motion Sensor Task

	Runs on your Gateway in the background and keeps track of 
	people near your machine.

  */


#ifndef _included_motionsensor_h
#define _included_motionsensor_h


#include "interface/taskmanager/uplinktask.h"



class MotionSensor : public UplinkTask
{

protected:

	static void SensorDraw ( Button *button, bool highlighted, bool clicked );
	static void SensorClose ( Button *button );

protected:

	int oldnumpeople;

public:

	MotionSensor ();
	~MotionSensor ();

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


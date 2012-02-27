
/*

  Revelation

	THE virus ;)

  */


#include "interface/taskmanager/uplinktask.h"


class Revelation : public UplinkTask
{

protected:

	time_t timesync;
    int animationtime;
    int nextnumber;
    int numcopies;

protected:

    static void DrawRevelation ( Button *button, bool highlighted, bool clicked );

public:

	void Initialise ();     
	void Tick ( int n );	

	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	bool IsInterfaceVisible ();    

};

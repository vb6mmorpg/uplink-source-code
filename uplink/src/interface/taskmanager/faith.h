
/*

  Faith

	The other virus ;)

  */


#include "interface/taskmanager/uplinktask.h"


class Faith : public UplinkTask
{

public:

	void Initialise ();     
	void Tick ( int n );	

	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	bool IsInterfaceVisible ();    

};

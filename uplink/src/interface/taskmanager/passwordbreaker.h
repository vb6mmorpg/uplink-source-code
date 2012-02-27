
#ifndef _included_passwordbreaker_h
#define _included_passwordbreaker_h


#include "vanbakel.h"

#include "interface/taskmanager/uplinktask.h"
#include "world/date.h"


class PasswordBreaker : public UplinkTask  
{

protected:

	int length, difficulty, currentchar;
	char *caption;
	char *username;
	char *password;
	bool *found;

	int numticksrequired;						// For next letter
	int progress;

protected:

	static void BackgroundDraw ( Button *button, bool highlighted, bool clicked );

	static void PasswordDraw ( Button *button, bool highlighted, bool clicked );
	static void PasswordClick ( Button *button );

	static void CloseClick ( Button *button );

public:

	PasswordBreaker();

	virtual ~PasswordBreaker();

	void SetTarget ( UplinkObject *uo, char *uos, int uoi );				

	void Initialise ();     // Called at creation time, neccisary
	void Tick ( int n );           // n ticks occur
	
	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	bool IsInterfaceVisible ();     

};

#endif


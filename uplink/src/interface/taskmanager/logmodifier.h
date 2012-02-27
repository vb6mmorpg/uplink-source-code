
/*

  Log Modifier software

	Allows the player to make changes
	to an existing log

  */


#include "eclipse.h"

#include "interface/taskmanager/uplinktask.h"

#define		LOGMODIFIER_STATUS_NONE			0
#define		LOGMODIFIER_STATUS_WAITING		1
#define		LOGMODIFIER_STATUS_ACTIVE		2						// (expanded)
#define		LOGMODIFIER_STATUS_MODIFYING	3
#define		LOGMODIFIER_STATUS_FINISHED		4

#define		LOGMODIFIER_STATUS_CREATING		5						// ( V2.0 )


class LogBank;



class LogModifier : public UplinkTask
{

protected:

	LogBank *source;	
	int sourceindex;

	int status;	
	int numticksrequired;
	int progress;

	int logtype;

protected:

	static void BorderDraw		( Button *button, bool highlighted, bool clicked );
	static void BackgroundDraw	( Button *button, bool highlighted, bool clicked );
	static void ProgressDraw	( Button *button, bool highlighted, bool clicked );

	static void BorderClick ( Button *button );
	static void CloseClick  ( Button *button );
	static void CommitClick ( Button *button );
	
	static void NextTypeClick ( Button *button );
	static void ChangeLogType ( int pid );								// Updates captions

public:

	LogModifier ();
	~LogModifier ();

	void SetTarget ( UplinkObject *uo, char *uos, int uoi );
	void MoveTo ( int x, int y, int time_ms );							// Origin top left		

	void Initialise ();     
	void Tick ( int n );    
	
	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	bool IsInterfaceVisible ();     

	void CreateExpandedInterface ();
	void RemoveExpandedInterface ();
	bool IsExpandedInterfaceVisible ();	
	
};

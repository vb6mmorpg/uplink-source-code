
#ifndef _included_uplinkagentlist_h
#define _included_uplinkagentlist_h


#include "vanbakel.h"

#include "interface/taskmanager/uplinktask.h"



class UplinkAgentList : public UplinkTask  
{


protected:

	static void UplinkAgentListDraw ( Button *button, bool highlighted, bool clicked );	
	
    static void ScrollUpClick ( Button *button );
    static void ScrollDownClick ( Button *button );
    static void TitleClick ( Button *button );
    static void CloseClick ( Button *button );

protected:

    DArray <char *> handles;
    DArray <char *> names;

    static int baseoffset;

public:

	UplinkAgentList();
	virtual ~UplinkAgentList();

    void UpdateLists ();

	void SetTarget ( UplinkObject *uo, char *uos, int uoi );				

	void Initialise ();     
	void Tick ( int n );    

	void MoveTo ( int x, int y, int time_ms );			
    
	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	bool IsInterfaceVisible ();     

};

#endif


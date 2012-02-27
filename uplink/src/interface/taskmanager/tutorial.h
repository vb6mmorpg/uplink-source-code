

/*

  Tutorial program

	Sits on screen and tells you what to do 
	every now and again


  */

#ifndef _included_tutorial_h
#define _included_tutorial_h

#include "eclipse.h"

#include "interface/taskmanager/uplinktask.h"


class Tutorial : public UplinkTask
{

protected:

	static void CloseClick ( Button *button );
	static void TitleClick ( Button *button );
	static void SkipClick ( Button *button );

	static void NextDraw ( Button *button, bool highlighted, bool clicked );
	static void NextMouseMove ( Button *button );
	static void NextMouseDown ( Button *button );
	static void NextClick ( Button *button );

	static void MainTextDraw ( Button *button, bool highlighted, bool clicked );

	static void MenuButtonClick ( Button *button );

protected:

	int current_tutorial;
	int current_part;
	
	bool nextvisible;									// True if the NEXT button should be visible
	bool nextclicked;									// True if the NEXT button has been clicked
	bool menu;											// True if we're in the selection menu

public:

	Tutorial ();
	~Tutorial ();

	void RunMenu ();
	void RunTutorial ( int tutorial, int part );
    void RunTutorial1 ( int part );
    void RunTutorial2 ( int part );
    void RunTutorial3 ( int part );
	bool HasCompletedCurrentSection ();
	

	void Initialise ();     
	void Tick ( int n );           

	void MoveTo ( int x, int y, int time_ms );			

	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();
	bool IsInterfaceVisible ();     

	void MoveMenuTo ( int x, int y, int time_ms );

	void CreateMenuInterface ();
	void RemoveMenuInterface ();
	void ShowMenuInterface ();
	bool IsMenuInterfaceVisible ();

};



#endif



/*

  Faith

	The other virus ;)

  */


#ifndef _virus_included_h
#define _virus_included_h

#include "interface/taskmanager/uplinktask.h"

#define VIRUS_IDLE						0
#define VIRUS_WORKING					1	// For single target viruses
#define VIRUS_SCANNING					2	// For databank scanning viruses

class DataBank;
class Data;

class Virus : public UplinkTask
{
protected:

	DataBank *source;	
	int sourceindex;

	int downloading;				// 0 = not, 1 = in progress, 2 = download complete, 3 = finished
    bool remotefile;                // True if the target is coming in remotely
	
	int numticksrequired;
	int progress;					// 0.0 - numticksrequired
	int ticksdone;
	int currentMemorySlot;

public:

	Virus ();
	Virus ( int newvirustype );

	void Initialise ();     
	void Tick ( int n );	

	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	void SetTarget ( UplinkObject *uo, char *uos, int uoi );
	void MoveTo ( int x, int y, int time_ms );				// Centred on top-right of title image

	static void BorderDraw    ( Button *button, bool highlighted, bool clicked );
	static void ProgressDraw  ( Button *button, bool highlighted, bool clicked );

	static void CloseClick    ( Button *button );
	static void BorderClick   ( Button *button );


	bool IsInterfaceVisible ();    

	void PornEffect ( DataBank *databank );       
	void FormatEffect ( DataBank *databank);       
	void CryptEffect ( Data *data );       
	void CorruptEffect ( Data *data );       
	void HandleNewMemorySlot ( DataBank *databank, int index, bool fill, bool format );

protected:

	int virustype;

};

#endif

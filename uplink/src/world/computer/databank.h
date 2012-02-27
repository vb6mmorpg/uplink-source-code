
/*

  DataBank class object

	Represents the data storage of a person, computer, message etc.

  */


#ifndef _included_databank_h
#define _included_databank_h


#include "app/uplinkobject.h"

class Data;



class DataBank : public UplinkObject  
{

protected:

	DArray <Data *> data;					// All files
	DArray <int>	memory;					// indexes into data (ie FAT)

public:

	bool formatted;							// Set if databank was recently wiped

public:

	DataBank();
	virtual ~DataBank();

	void SetSize ( int newsize );
	int  GetSize ();										
	
	int  NumDataFiles ();
	int  GetDataSize ();

	bool PutData ( Data *newdata );                                 // Return true if the data was inserted
    void InsertData ( Data *newdata );                              // Inserts at a random position
    void PutData ( Data *newdata, int memoryindex );				// Overwrites 
    
	void RemoveData ( int memoryindex );							// Removes the file from data as well
	void RemoveDataFile ( int dataindex );							// Removes all references in memory as well

	int IsValidPlacement ( Data *newdata, int memoryindex );		// 0 = yes, 1 = will overwrite, 2 = no
	int FindValidPlacement ( Data *newdata );						// -1 = failure

	Data *GetData     ( int memoryindex );
	Data *GetDataFile ( int dataindex );
	Data *GetData     ( char *title );
	bool ContainsData ( char *title, float version = -1.0f );

	int GetDataIndex ( int memoryindex );						// Returns index of data in this memory block
	int GetMemoryIndex ( int dataindex );						// Finds first memory index pointing to the data

	void Format ();												// Wipes everything

	void RandomizeDataPlacement ();								// Change the placement of the files on the server

	// Common functions

	bool Load  ( FILE *file );			
	void Save  ( FILE *file );			
	void Print ();		
	void Update ();			
	
	char *GetID ();	
	int GetOBJECTID ();

};


// ============================================================================


#define SIZE_DATA_TITLE		64


#define     DATATYPE_NONE			0
#define     DATATYPE_DATA			1
#define     DATATYPE_PROGRAM		2
	
#define		SOFTWARETYPE_NONE		0
#define		SOFTWARETYPE_FILEUTIL	1
#define		SOFTWARETYPE_HWDRIVER	2
#define		SOFTWARETYPE_SECURITY	3
#define		SOFTWARETYPE_CRACKERS	4
#define		SOFTWARETYPE_BYPASSER	5
#define		SOFTWARETYPE_LANTOOL	6
#define		SOFTWARETYPE_HUDUPGRADE 9
#define		SOFTWARETYPE_OTHER		10


class Data : public UplinkObject
{

public:

	char title [SIZE_DATA_TITLE];
	int TYPE;
	int size;
	int encrypted;						// 0 = not encrypted, >=1 = encrypted
	int compressed;						// 0 = not compressed, >=1 = compressed 
	
	float version;						
	int softwareTYPE;						

public:

	Data ();
	Data ( Data *copyme );
	~Data ();

	void SetTitle ( char *newtitle );
	void SetDetails ( int newTYPE, int newsize, 
					  int newencrypted = 0, int newcompressed = 0, 
					  float newversion = 1.0, int newsoftwareTYPE = SOFTWARETYPE_NONE );

	// Common functions

	bool Load  ( FILE *file );			
	void Save  ( FILE *file );			
	void Print ();		
	void Update ();			
	
	char *GetID ();	
	int GetOBJECTID ();

};

#endif 

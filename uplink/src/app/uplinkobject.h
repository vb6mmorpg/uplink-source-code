/*

  UplinkObject base class

	All uplink classes should be derived from this
	and should implement Load, Save and Print.  Update is optional.

  */

#ifndef _included_uplinkobject_h
#define _included_uplinkobject_h

#include <stdio.h>

#include "tosser.h"

#define SIZE_SAVEID     9
#define SIZE_SAVEID_END 13


class UplinkObject  
{

public:

	UplinkObject();
	virtual ~UplinkObject();

	virtual bool Load   ( FILE *file );
	virtual void Save   ( FILE *file );
	virtual void Print  ();
	virtual void Update ();
	
	virtual char *GetID ();						// You must provide this
	virtual int   GetOBJECTID ();				// Neccisary if this is used in a Tosser structure

	char *GetID_END ();							//
	void LoadID ( FILE *file );					// Used to ensure
	void SaveID ( FILE *file );					// VERY safe loading
	void LoadID_END ( FILE *file );				// of save game files		
	void SaveID_END ( FILE *file );				//

};


#endif 

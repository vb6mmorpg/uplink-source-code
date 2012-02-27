
#include <unrar/rarbloat.h>
#include <unrar/sha1.h>

#include "mmgr.h"

void *HashInitial ()
{

	hash_context *c = new hash_context;
	hash_initial( c );

	return c;

}

void HashData ( void *context, unsigned char *data, unsigned int lendata )
{

	hash_context *c = (hash_context *) context;

	hash_process ( c, data, lendata );

}

unsigned int HashResultSize ()
{

	return ( HW * sizeof ( uint32 ) );

}

unsigned int HashFinal ( void *context, unsigned char *result, unsigned int lenresult )
{

	hash_context *c = (hash_context *) context;

    uint32 hash [ HW ];
	hash_final ( c, hash );
	delete c;
	
	unsigned int hashsize = HW * sizeof ( uint32 );
	unsigned int lencopy = ( hashsize > lenresult ) ? lenresult : hashsize;

	memcpy ( result, hash, lencopy );
	return lencopy;

    //sprintf( _result, "hsh%04x%04x%04x%04x%04x", hash[0], hash[1], hash[2], hash[3], hash[4] );

}


#ifndef _included_hash_h
#define _included_hash_h

/*
 *	Hash data using SHA1
 *  Can pass in a seed hash token
 *
 *  result is written into
 */


// Call once at the beginning of the hashing, return the context
void        *HashInitial    ();

// Hash data, can be called multiple times
void         HashData       ( void *context, unsigned char *data, unsigned int lendata );

// Returns the hash result size
unsigned int HashResultSize ();

// Call once at the end of the hashing, write the hash in result and return the number of char written to result
unsigned int HashFinal      ( void *context, unsigned char *result, unsigned int lenresult );


#endif

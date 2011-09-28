
/*

  Redshirt command line application
  For encrypting/decrypting data files

  */

#include <stdlib.h>

#include "redshirt.h"


void incorrectusage ( char *filename )
{

	printf ( "Usage :\n" );
	printf ( "to Encrypt : %s -e filename\n", filename );
	printf ( "to Decrypt : %s -d filename\n", filename );
	printf ( "\n" );

	exit (1);

}

void encrypt ( char *filename )
{

	printf ( "Encrypting file : %s ... ", filename );

	bool success = RsEncryptFile ( filename );

	if ( success )  printf ( "done\n" );
	else			printf ( "failed\n" );

}

void decrypt ( char *filename )
{

	printf ( "Decrypting file : %s ... ", filename );

	bool success = RsDecryptFile ( filename );

	if ( success )  printf ( "done\n" );
	else			printf ( "failed\n" );

}

int main ( int argc, char *argv [] )
{

	//
	// Introduction
	//
		
	printf ( "RedShirt command line utility\n" );

	if ( argc < 3 ) incorrectusage ( ( argc > 0 )? argv[0] : "redshirt" );

	//
	// Parse command line options
	//

	char option;
	char filename [256];
	memset ( filename, 0, sizeof(filename) );

	sscanf ( argv [1], "-%c", &option );
	sscanf ( argv [2], "%255s", filename );

	if ( option != 'e' && option != 'd' ) incorrectusage ( ( argc > 0 )? argv[0] : "redshirt" );

	//
	// Act on options
	//

	bool success = false;

	if		( option == 'e' ) encrypt ( filename );
	else if ( option == 'd' ) decrypt ( filename );

	return 0;
	
}

